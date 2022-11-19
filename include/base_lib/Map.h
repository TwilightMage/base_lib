#pragma once

#include <stdexcept>

#include "List.h"
#include "Pair.h"
#include "Tree1D.h"

template<typename Key, typename Value>
class Map
{
public:
    Map()
    {
    }

    Map(const std::initializer_list<KeyValuePair<Key, Value>>& initializer_list)
    {
        for (const auto& pair : initializer_list)
        {
            insert(pair.key, pair.value);
        }
    }
    
    void insert(const Key& key, const Value& new_value)
    {
        data_.insert(KeyValuePair<Key, Value>(key, new_value));
    }

    Value& find_or_insert(const Key& key, std::function<Value()> generator)
    {
        if (auto value = data_.find(KeyValuePair<Key, Value>(key)))
        {
            return *value;
        }

        return data_.insert(KeyValuePair<Key, Value>(key, generator())).value;
    }
    
    Value& find_or_insert(const Key& key, const Value& new_value = Value())
    {
        if (auto value = data_.find(KeyValuePair<Key, Value>(key)))
        {
            return *value;
        }

        return data_.insert(KeyValuePair<Key, Value>(key, new_value)).value;
    }

    Value& operator[](const Key& key)
    {
        return at(key);
    }

    const Value& operator[](const Key& key) const
    {
        return at(key);
    }

    Value& at(const Key& key)
    {
        if (auto value = data_.find(KeyValuePair<Key, Value>(key)))
        {
            return value->value;
        }

        return data_.insert(KeyValuePair<Key, Value>(key, Value())).value;
    }

    const Value& at(const Key& key) const
    {
        if (auto value = data_.find(KeyValuePair<Key, Value>(key)))
        {
            return value->value;
        }

        throw std::runtime_error("Unable to find value in map");
    }

    bool contains(const Key& key) const
    {
        return data_.find(KeyValuePair<Key, Value>(key)) != nullptr;
    }

    void remove(const Key& key)
    {
        data_.remove(KeyValuePair<Key, Value>(key));
    }

    void clear()
    {
        data_.clear();
    }

    Value* find(const Key& key) const
    {
        if (auto found = data_.find(KeyValuePair<Key, Value>(key)))
        {
            return &found->value;
        }
        else
        {
            return nullptr;
        }
    }

    Value find_or_default(const Key& key) const
    {
        if (auto found = data_.find(KeyValuePair<Key, Value>(key)))
        {
            return found->value;
        }
        else
        {
            return Value();
        }
    }

    Value find_or_default(const Key& key, const Value& default_value) const
    {
        if (auto found = data_.find(KeyValuePair<Key, Value>(key)))
        {
            return found->value;
        }
        else
        {
            return default_value;
        }
    }

    List<Key> get_keys() const requires std::is_default_constructible<Key>::value
    {
        List<Key> result;
        for (auto& entry : data_)
        {
            result.add(entry.key);
        }

        return result;
    }

    List<Value> get_values() const requires std::is_default_constructible<Value>::value
    {
        List<Value> result;
        for (auto& entry : data_)
        {
            result.add(entry.value);
        }

        return result;
    }

    uint size() const
    {
        return data_.size();
    }

    typename Tree1D<KeyValuePair<Key, Value>>::Iterator begin() const
    {
        return data_.begin();
    }

    typename Tree1D<KeyValuePair<Key, Value>>::Iterator end() const
    {
        return data_.end();
    }

    void write_to_stream(std::ostream& stream) const requires Serializable<Key> && Serializable<Value>
    {
        data_.write_to_stream(stream);
    }

    void read_from_stream(std::istream& stream) requires Serializable<Key> && Serializable<Value>
    {
        data_.read_from_stream(stream);
    }

private:
    Tree1D<KeyValuePair<Key, Value>> data_;
};
