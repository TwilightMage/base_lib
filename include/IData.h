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