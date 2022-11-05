#pragma once

#include <cstdint>
#include <memory>

#undef min
#undef max

typedef unsigned int uint;

typedef int32_t int32;
typedef uint32_t uint32;

typedef int64_t int64;
typedef uint64_t uint64;

typedef uint8_t byte;

typedef int16_t char16;
typedef uint16_t byte16;

#define KINDA_SMALL_NUMBER 0.0001f

/*template<typename T, class Deleter = std::default_delete<T>>
using Unique = std::unique_ptr<T, Deleter>;

template<typename T>
using Weak = std::weak_ptr<T>;

#define null_weak(Type) (std::weak_ptr<Type>())

template<typename T>
using Shared = std::shared_ptr<T>;

template<class T, class... Types>
__forceinline Shared<T> MakeShared(Types&&... Args)
{
	return std::make_shared<T>(std::forward<Types>(Args)...);
}*/
