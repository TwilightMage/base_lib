#pragma once

#pragma warning( disable : 4251 )
#pragma warning( disable : 4193 )
#pragma warning( disable : 4267 )
#pragma warning( disable : 4275 )

// ------------------------ Defines ------------------------
#define STRINGIFY(a) #a

// Misc definitions
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__))

#define DLL 1

#if DLL
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT
#endif

// Platform-specific definitions
#if PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
#endif



// ------------------------- Libs --------------------------

// Platform-specific libs
#if PLATFORM_WINDOWS
    #pragma comment(lib, "DbgHelp.lib")
#endif



// ----------------------- Includes ------------------------
// Platform-specific includes
#if PLATFORM_WINDOWS
    #include <windows.h>
#endif

#include <stdexcept>
#include "BasicTypes.h"


// -------------------- Functionality ----------------------
// memcpy   - when you move data backward: 0000xxxx0000 -> 00xxxx000000
// memcpy_b - when you move data forward:  0000xxxx0000 -> 000000xxxx00
// Same as memcpy, but works in reverse order
inline void memcpy_b(void* dst, void* src, size_t size)
{
    auto _dst = static_cast<byte*>(dst);
    auto _src = static_cast<byte*>(src);

    for (size_t i = 1; i <= size; i++)
    {
        _dst[size - i] = _src[size - i];
    }
}

/** Reinterpret object, use when you know what you're doing */
template<typename To, typename From>
To& cast_object(From& rhs)
{
	static_assert(sizeof(To) == sizeof(From), "Types have different size");
	return *reinterpret_cast<To*>(&rhs);
}

/** Reinterpret object, use when you know what you're doing */
template<typename To, typename From>
const To& cast_object(const From& rhs)
{
	static_assert(sizeof(To) == sizeof(From), "Types have different size");
	return *reinterpret_cast<const To*>(&rhs);
}

/** Reinterpret object without size check, use when you really know what you're doing */
template<typename To, typename From>
To& cast_object_unsafe(From& rhs)
{
	return *reinterpret_cast<To*>(&rhs);
}

/** Reinterpret object without size check, use when you really know what you're doing */
template<typename To, typename From>
const To& cast_object_unsafe(const From& rhs)
{
	return *reinterpret_cast<const To*>(&rhs);
}

/** Access any member only by it's byte offset */
template<typename MemberType, typename ObjectType>
MemberType* hack_member(ObjectType* obj, uint byte_offset)
{
	return reinterpret_cast<MemberType*>(reinterpret_cast<byte*>(obj) + byte_offset);
}

class NotImplementedException: public std::runtime_error {};



// --------------------- Enum macros -----------------------
#define IMPLEMENT_ENUM(type)										\
inline type& operator++(type& in) /*prefix ++*/						\
{																	\
	in = static_cast<type>(static_cast<uint32_t>(in) + 1);			\
	return in;														\
}																	\
inline type& operator--(type& in) /*prefix --*/						\
{																	\
	in = static_cast<type>(static_cast<uint32_t>(in) - 1);			\
	return in;														\
}																	\
inline type operator++(type& in, int) /*postfix ++*/				\
{																	\
	type tmp = in;													\
	++in;															\
	return tmp;														\
}																	\
inline type operator--(type& in, int) /*postfix --*/				\
{																	\
	type tmp = in;													\
	--in;															\
	return tmp;														\
}																	\
inline type operator+(type in, int delta)							\
{																	\
	return static_cast<type>(static_cast<uint32_t>(in) + delta);	\
}																	\
inline type operator-(type in, int delta)							\
{																	\
	return static_cast<type>(static_cast<uint32_t>(in) - delta);	\
}

#define IMPLEMENT_ENUM_FLAGS(Type)																											   \
inline           Type& operator|=(Type& lhs, Type rhs)   { return lhs = (Type)((__underlying_type(Type))lhs | (__underlying_type(Type))rhs); } \
inline           Type& operator&=(Type& lhs, Type rhs)   { return lhs = (Type)((__underlying_type(Type))lhs & (__underlying_type(Type))rhs); } \
inline           Type& operator^=(Type& lhs, Type rhs)   { return lhs = (Type)((__underlying_type(Type))lhs ^ (__underlying_type(Type))rhs); } \
inline constexpr Type  operator| (Type  lhs, Type rhs)   { return (Type)((__underlying_type(Type))lhs | (__underlying_type(Type))rhs); }       \
inline constexpr Type  operator& (Type  lhs, Type rhs)   { return (Type)((__underlying_type(Type))lhs & (__underlying_type(Type))rhs); }       \
inline constexpr Type  operator^ (Type  lhs, Type rhs)   { return (Type)((__underlying_type(Type))lhs ^ (__underlying_type(Type))rhs); }       \
inline constexpr bool  operator! (Type  e)               { return !(__underlying_type(Type))e; }                                               \
inline constexpr Type  operator~ (Type  e)               { return (Type)~(__underlying_type(Type))e; }                                         \
inline constexpr Type operator<< (Type  lhs, int offset) { return (Type)((__underlying_type(Type))lhs << offset); }                            \
inline constexpr Type operator>> (Type  lhs, int offset) { return (Type)((__underlying_type(Type))lhs >> offset); }