#pragma once

#include "List.h"
#include "Pair.h"

template<typename K, typename V>
struct SimpleMap
{
    SimpleMap()
    {
    }

    SimpleMap(const std::initializer_list<Pair<K, V>>& rhs)
    {
        for (const Pair<K, V>& entry : rhs)
        {
            operator[](entry.key) = entry.value;
        }
    }

    SimpleMap(const List<Pair<K, V>>& rhs)
    {
        for (const Pair<K, V>& entry : rhs)
        {
            operator[](entry.key) = entry.value;
        }
    }

    SimpleMap(const SimpleMap& rhs)
        : entries(rhs.size())
    {
        uint i = 0;
        for (Pair<K, V>* entry : rhs.entries)
        {
            entries[i++] = new Pair<K, V>(*entry);
        }
    }

    FORCEINLINE SimpleMap copy() const
    {
        return *this;
    }
    
    FORCEINLINE V& operator[](const K& key)
    {
        return at(key);
    }

    FORCEINLINE const V& operator[](const K& key) const
    {
        return at(key);
    }

    FORCEINLINE void clear()
    {
        /*for (uint i = 0; i < entries.length(); i++) delete entries[i];*/
        entries.clear();
    }

    FORCEINLINE Pair<K, V>** begin()
    {
        return entries.begin();
    }

    FORCEINLINE Pair<K, V>** end()
    {
        return entries.end();
    }

    FORCEINLINE const Pair<K, V>* const* begin() const
    {
        return entries.begin();
    }

    FORCEINLINE const Pair<K, V>* const* end() const
    {
        return entries.end();
    }

    FORCEINLINE uint size() const
    {
        return entries.length();
    }

    void remove(const K& key)
    {
        uint l = 0;
        uint r = entries.length();
        uint m = 0;

        while (l < r)
        {
            m = (l + r) / 2;
            if (entries[m]->key < key)
            {
                l = m + 1;
            }
            else if (entries[m]->key > key)
            {
                r = m;
            }
            else
            {
                entries.remove_at(m);
                return;;
            }
        }
    }

    bool contains(const K& key) const
    {
        uint l = 0;
        uint r = entries.length();
        uint m = 0;

        while (l < r)
        {
            m = (l + r) / 2;
            if (entries[m]->key < key)
            {
                l = m + 1;
            }
            else if (entries[m]->key > key)
            {
                r = m;
            }
            else
            {
                return true;
            }
        }

        return false;
    }

    V& at(const K& key)
    {
        uint l = 0;
        uint r = entries.length();
        uint m = 0;

        while (l < r)
        {
            m = (l + r) / 2;
            if (entries[m]->key < key)
            {
                l = m + 1;
            }
            else if (entries[m]->key > key)
            {
                r = m;
            }
            else
            {
                return entries[m]->value;
            }
        }

        m = (l + r) / 2;
        
        entries.insert(new Pair<K, V>(key, V()), m);
        return entries[m]->value;
    }

    const V& at(const K& key) const
    {
        uint l = 0;
        uint r = entries.length();
        uint m = 0;

        while (l < r)
        {
            m = (l + r) / 2;
            if (entries[m]->key < key)
            {
                l = m + 1;
            }
            else if (entries[m]->key > key)
            {
                r = m;
            }
            else
            {
                return entries[m]->value;
            }
        }

        throw std::runtime_error("Unable to find value in simple map");
    }

    V* find(const K& key) const
    {
        uint l = 0;
        uint r = entries.length();
        uint m = 0;

        while (l < r)
        {
            m = (l + r) / 2;
            if (entries[m]->key < key)
            {
                l = m + 1;
            }
            else if (entries[m]->key > key)
            {
                r = m;
            }
            else
            {
                return &entries[m]->value;
            }
        }

        return nullptr;
    }

    List<K> get_keys() const requires std::is_default_constructible<K>::value
    {
        List<K> result;

        for (auto& entry : entries)
        {
            result.add(entry->key);
        }

        return result;
    }

    List<V> get_values() const
    {
        List<V> result;

        for (auto& entry : entries)
        {
            result.add(entry->value);
        }

        return result;
    }

    SimpleMap& chain(const K& key, const V& value)
    {
        operator[](key) = value;

        return *this;
    }

    SimpleMap chain(const K& key, const V& value) const
    {
        SimpleMap result = *this;

        result[key] = value;

        return result;
    }

    void write_to_stream(std::ostream& stream) const requires Serializable<K> && Serializable<V>
    {
        StreamUtils::write(stream, entries.length());
        for (uint i = 0; i < entries.length(); i++)
        {
            StreamUtils::write(stream, *entries[i]);
        }
    }

    void read_from_stream(std::istream& stream) requires Serializable<K> && Serializable<V>
    {
        entries.resize(StreamUtils::read<uint>(stream));
        for (uint i = 0; i < entries.length(); i++)
        {
            if (entries[i] == nullptr) entries[i] = new Pair<K, V>();
            StreamUtils::read(stream, *entries[i]);
        }
    }

    void merge(SimpleMap right, bool prefer_right = true)
    {
        if (prefer_right)
        {
            for (const auto& right_entry : right.entries)
            {
                operator[](right_entry->key) = right_entry->value;
            }
        }
        else
        {
            for (const auto& right_entry : right.entries)
            {
                if (!contains(right_entry->key))
                {
                    operator[](right_entry->key) = right_entry->value;
                }
            }
        }
    }

    SimpleMap& operator=(const SimpleMap& rhs)
    {
        if (*this == rhs) return *this;

        entries = rhs.entries;

        return *this;
    }

    bool operator==(const SimpleMap& rhs) const
    {
        return entries == rhs.entries;
    }

    List<Pair<K, V>*> entries;
};
