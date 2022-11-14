#pragma once

#include "Utils.h"
#include "Logger.h"

#include <assert.h>

#if DEBUG
    #define Assert(condition)\
    if (!(condition))\
    {\
        print_error("Assert", "Failed %s\n%s", #condition, Utils::get_callstack_string().c());\
        _wassert(_CRT_WIDE(#condition), _CRT_WIDE(__FILE__), (unsigned)(__LINE__));\
    }

    #define AssertMsg(condition, category, error_message_format, ...)\
    if (!(condition))\
    {\
        print_error(#category, "%s\n%s", String::format(error_message_format, __VA_ARGS__).c(), Utils::get_callstack_string().c());\
        _wassert(String::format(error_message_format, __VA_ARGS__).wc(), _CRT_WIDE(__FILE__), (unsigned)(__LINE__));\
    }
#else
    #define Assert(condition)\
    if (!(condition))\
    {\
        print_error("Assert", "Failed %s\n%s", #condition, Utils::get_callstack_string().c());\
        exit(1);\
    }

    #define AssertMsg(condition, category, error_message_format, ...)\
    if (!(condition))\
    {\
        print_error(#category, "%s\n%s", String::format(error_message_format, __VA_ARGS__).c(), Utils::get_callstack_string().c());\
        exit(1);\
    }
#endif

template<typename... T>
FORCEINLINE bool CheckImpl(bool condition, const String& category, const String& error_message_format, T... params)
{
    if (!condition)
    {
        print_warning(category, error_message_format, std::forward<T>(params)...);
    }
    return condition;
}
#define Check(condition, category, error_message_format, ...) CheckImpl((condition), category, error_message_format, __VA_ARGS__)

template<typename... T>
FORCEINLINE bool CheckErrorImpl(bool condition, const String& category, const String& error_message_format, T... params)
{
    if (!condition)
    {
        print_error(category, error_message_format, std::forward<T>(params)...);
    }
    return condition;
}
#define CheckError(condition, category, error_message_format, ...) CheckErrorImpl((condition), category, error_message_format, __VA_ARGS__)
