#pragma once
#include "FastOperator.h"

template<typename K, typename V>
struct Pair
{
    K key;
    V value;

    Pair()
        : key(K())
        , value(V())
    {
    }

    Pair(const K& key, const V& value)
        : key(key)
        , value(value)
    {
    }
    
    FAST_OPERATOR_TWO_LESS(Pair, key, value)
    FAST_OPERATOR_TWO_LESS_EQUAL(Pair, key, value)
    FAST_OPERATOR_TWO_EQUAL(Pair, key, value)
    FAST_OPERATOR_TWO_GREATER_EQUAL(Pair, key, value)
    FAST_OPERATOR_TWO_GREATER(Pair, key, value)
    FAST_OPERATOR_TWO_NOT_EQUAL(Pair, key, value)

    void write_to_stream(std::ostream& stream) const requires Serializable<K> && Serializable<V>
    {
        StreamUtils::write(stream, key);
        StreamUtils::write(stream, value);
    }

    void read_from_stream(std::istream& stream) requires Serializable<K> && Serializable<V>
    {
        StreamUtils::read(stream, key);
        StreamUtils::read(stream, value);
    }
};
