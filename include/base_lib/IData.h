#pragma once

#include <concepts>
#include <type_traits>

#include "framework.h"

class EXPORT IData
{};

template<bool IsValid>
class EXPORT IDataChecked
{};

template<typename T>
concept Data = std::is_trivial<T>::value || std::derived_from<T, IData> || std::derived_from<T, IDataChecked<true>>;

template<typename A, typename B = A>
concept HaveLessOperator = requires(const A& a, const B& b)
{
    {a < b} -> std::same_as<bool>;
};

template<typename A, typename B = A>
concept CanCheckLess = HaveLessOperator<A, B> || Data<A> && Data<B> && std::same_as<A, B>;