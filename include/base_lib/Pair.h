#pragma once

#include "FastOperator.h"

template<typename K, typename V, bool compare_value>
struct PairBase {
    K key;
    V value;

    PairBase()
        : key(K())
        , value(V())
    {}

    explicit PairBase(const K& key)
        : key(key)
        , value(V())
    {}

    PairBase(const K& key, const V& value)
        : key(key)
        , value(value)
    {}

    bool operator<(const PairBase& rhs) const {
        if constexpr (compare_value) {
            return key < rhs.key || key == rhs.key && value < rhs.value;
        } else {
            return key < rhs.key;
        }
    }
    bool operator<=(const PairBase& rhs) const {
        if constexpr (compare_value) {
            return key < rhs.key || key == rhs.key && value <= rhs.value;
        } else {
            return key <= rhs.key;
        }
    }
    bool operator==(const PairBase& rhs) const {
        if constexpr (compare_value) {
            return key == rhs.key && value == rhs.value;
        } else {
            return key == rhs.key;
        }
    }
    bool operator>=(const PairBase& rhs) const {
        if constexpr (compare_value) {
            return key > rhs.key || key == rhs.key && value >= rhs.value;
        } else {
            return key >= rhs.key;
        }
    }
    bool operator>(const PairBase& rhs) const {
        if constexpr (compare_value) {
            return key > rhs.key || key == rhs.key && value > rhs.value;
        } else {
            return key > rhs.key;
        }
    }
    bool operator!=(const PairBase& rhs) const {
        if constexpr (compare_value) {
            return key != rhs.key || value != rhs.value;
        } else {
            return key != rhs.key;
        }
    }

    void write_to_stream(std::ostream& stream) const requires Serializable<K> && Serializable<V> {
        StreamUtils::write(stream, key);
        StreamUtils::write(stream, value);
    }

    void read_from_stream(std::istream& stream) requires Serializable<K> && Serializable<V> {
        StreamUtils::read(stream, key);
        StreamUtils::read(stream, value);
    }
};

template<typename KeyType, typename ValueType>
using Pair = PairBase<KeyType, ValueType, true>;

template<typename KeyType, typename ValueType>
using KeyValuePair = PairBase<KeyType, ValueType, false>;
