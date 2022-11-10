#pragma once

#include <iostream>

#include "IData.h"

class EXPORT ISerializable
{
public:
    virtual void write_to_stream(std::ostream& stream) const = 0;
    virtual void read_from_stream(std::istream& stream) = 0;
};

template<typename T>
concept ImplicitlySerializable = requires(const T a1, T a2, std::ostream& os, std::istream& is)
{
    a1.write_to_stream(os);
    a2.read_from_stream(is);
};

template<typename T>
concept ExplicitlySerializable = std::derived_from<T, ISerializable>;

template<typename T>
concept Serializable = Data<T> || ExplicitlySerializable<T> || ImplicitlySerializable<T>;