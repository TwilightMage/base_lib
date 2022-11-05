#pragma once

#include <valarray>

#include "framework.h"

#undef min
#undef max

class Math
{
public:
    template<typename T>
    FORCEINLINE static T pi()
    {
        return static_cast<T>(3.141592653589793238462643383279502884197169399375105820974944592);
    }

    template<typename T>
    FORCEINLINE static T epsilon()
    {
        return static_cast<T>(2.2204460492503131e-016);
    }

    template<typename T>
    FORCEINLINE static T min(T a, T b)
    {
        return a < b ? a : b;
    }

    template<typename T>
    FORCEINLINE static T max(T a, T b)
    {
        return a > b ? a : b;
    }

    template<typename T>
    FORCEINLINE static T clamp(T value, T min, T max)
    {
        return value < min ? min : value > max ? max : value;
    }

    template<typename T>
    static T unwind(T value, T min, T max)
    {
        while (value >= max) value -= max - min;
        while (value < min) value += max - min;

        return value;
    }

    FORCEINLINE static uint mod(int value, uint m)
    {
        int mod = value % (int)m;
        if (mod < 0) {
            mod += m;
        }
        return mod;
    }

    template<typename T>
    FORCEINLINE static T lerp(T from, T to, float alpha)
    {
        return from + (to - from) * clamp(alpha, 0.0f, 1.0f);
    }

    template<typename T>
    FORCEINLINE static T sign(T in)
    {
        return in == 0 ? 0 : in / abs(in);
    }

    template<typename T>
    FORCEINLINE static T abs(T value)
    {
        return std::abs(value);
    }

    template<typename T>
    FORCEINLINE static T degrees(T radians)
    {
        return radians / pi<T>() * static_cast<T>(180);
    }

    template<typename T>
    FORCEINLINE static T radians(T degrees)
    {
        return degrees / static_cast<T>(180) * pi<T>();
    }
    
    template<typename T>
    FORCEINLINE static T sqr(T in)
    {
        return in * in;
    }

    template<typename T>
    FORCEINLINE static T pow(T in, T pow)
    {
        return std::pow(in, pow);
    }

    constexpr static uint pow_of_two(uint pow)
    {
        return 1 << pow;
    }

    template<typename T>
    FORCEINLINE static T log2(T from)
    {
        return std::log2(from);
    }

    template<typename T>
    FORCEINLINE static T sqrt(T in)
    {
        return std::sqrt(in);
    }

    template<typename T>
    FORCEINLINE static T sin_deg(T deg)
    {
        return sin(radians(deg));
    }

    template<typename T>
    FORCEINLINE static T sin_rad(T rad)
    {
        return sin(rad);
    }

    template<typename T>
    FORCEINLINE static T cos_deg(T deg)
    {
        return cos(radians(deg));
    }

    template<typename T>
    FORCEINLINE static T cos_rad(T rad)
    {
        return cos(rad);
    }

    template<typename T>
    FORCEINLINE static T acos_deg(T in)
    {
        return degrees(std::acos(in));
    }

    template<typename T>
    FORCEINLINE static T acos_rad(T in)
    {
        return std::acos(in);
    }

    template<typename T>
    static T unwind_angle(T in)
    {
        while (in >= 360) in -= 360;
        while (in < 0) in += 360;

        return in;
    }

    template<typename T>
    FORCEINLINE static int floor(T in)
    {
        return static_cast<int>(::floor(in));
    }

    template<typename T>
    FORCEINLINE static int ceil(T in)
    {
        return static_cast<int>(::ceil(in));
    }

    template<typename T>
    FORCEINLINE static int round(T in)
    {
        return static_cast<int>(::round(in));
    }
};
