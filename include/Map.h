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

    Map(const std::initializer_list<Pair<Key, Value>>& initializer_list)
    {
        for (const auto& pair : initializer_list)
        {
            insert(pair.key, pair.value);
        }
    }
    
    FORCEINLINE void insert(const Key& key, const Value& new_value)
    {
        data_.insert(key, new_value);
    }

    Value& find_or_insert(const Key& key, std::function<Value()> generator)
    {
        if (auto value = data_.find(key))
        {
            return *value;
        }
        
        data_.insert(key, generator());
        return *data_.find(key);
    }
    
    Value& find_or_insert(const Key& key, const Value& new_value = Value())
    {
        if (auto value = data_.find(key))
        {
            return *value;
        }
        
        data_.insert(key, new_value);
        return *data_.find(key);
    }

    FORCEINLINE Value& operator[](const Key& key)
    {
        return at(key);
    }

    FORCEINLINE const Value& operator[](const Key& key) const
    {
        return at(key);
    }

    Value& at(const Key& key)
    {
        if (auto value = data_.find(key))
        {
            return *value;
        }
        
        data_.insert(key, Value());
        return *data_.find(key);
    }

    const Value& at(const Key& key) const
    {
        if (auto value = data_.find(key))
        {
            return *value;
        }

        throw std::runtime_error("Unable to find value in map");
    }

    FORCEINLINE bool contains(const Key& key) const
    {
        return data_.find(key) != nullptr;
    }

    FORCEINLINE void remove(const Key& key)
    {
        data_.remove(key);
    }

    FORCEINLINE void clear()
    {
        data_.clear();
    }

    FORCEINLINE Value* find(const Key& key) const
    {
        return data_.find(key);
    }

    Value find_or_default(const Key& key) const
    {
        if (auto found = data_.find(key))
        {
            return *found;
        }
        else
        {
            return Value();
        }
    }

    Value find_or_default(const Key& key, const Value& default_value) const
    {
        if (auto found = data_.find(key))
        {
            return *found;
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

    FORCEINLINE uint size() const
    {
        return data_.size();
    }

    FORCEINLINE typename Tree1D<Key, Value>::Iterator begin() const
    {
        return data_.begin();
    }

    FORCEINLINE typename Tree1D<Key, Value>::Iterator end() const
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
    Tree1D<Key, Value> data_;
};
