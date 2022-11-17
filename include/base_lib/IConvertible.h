#pragma once

#include <concepts>

template<typename T>
class IConvertible
{
public:
    virtual void convert_to(T& to) const = 0;
    virtual void convert_from(const T& from) = 0;
};

template<typename FromType, typename ToType>
concept ImplicitlyConvertible = requires(const FromType from_const, FromType from, const ToType to_const, ToType to)
{
    from_const.convert_to(to);
    from.convert_from(to_const);
};

template<typename FromType, typename ToType>
concept ExplicitConversionOperator = requires(const FromType a)
{
    { (ToType)a } -> std::same_as<ToType>;
};

template<typename FromType, typename ToType>
concept Convertible = std::derived_from<FromType, IConvertible<ToType>> || ImplicitlyConvertible<FromType, ToType> || std::derived_from<ToType, IConvertible<FromType>> || std::same_as<FromType, ToType> || std::assignable_from<ToType, FromType> || std::constructible_from<ToType, FromType> || ExplicitConversionOperator<FromType, ToType>;

template<typename FromType, typename ToType>
void convert(const FromType& from, ToType& to) requires Convertible<FromType, ToType>
{
    if ((void*)&from == (void*)&to) return;
    
    if constexpr (std::derived_from<FromType, IConvertible<ToType>>)
    {
        from.convert_to(to);
    }
    else if constexpr (std::derived_from<ToType, IConvertible<FromType>>)
    {
        to.convert_from(from);
    }
    else if constexpr (std::same_as<FromType, ToType>)
    {
        to = from;
    }
    else if constexpr (std::assignable_from<ToType, FromType>)
    {
        to = from;
    }
    else if constexpr (std::constructible_from<ToType, FromType>)
    {
        to = ToType(from);
    }
    else if constexpr (ExplicitConversionOperator<FromType, ToType>)
    {
        to = (ToType)from;
    }
}