#pragma once

#include <random>

#include "framework.h"

#define RANDOM_MAX 2147483647

typedef std::linear_congruential_engine<unsigned int, 48271, 0, 2147483647> random_engine;

class Random
{
public:
    Random(uint seed)
    {
        rd = std::minstd_rand(seed);
    }

    template<typename T>
    T number()
    {
        return rd() % RANDOM_MAX;
    }
    template<typename T>
    T number(T max)
    {
        return rd() % max;
    }
    template<typename T>
    T number(T min, T max)
    {
        return min + rd() % (max - min);
    }

    bool boolean()
    {
        return rd() % 2 == 1;
    }
    
    template<typename T>
    static T static_number()
    {
        return static_rd() % RANDOM_MAX;
    }
    template<typename T>
    static T static_number(T max)
    {
        return static_rd() % max;
    }
    template<typename T>
    static T static_number(T min, T max)
    {
        return min + static_rd() % (max - min);
    }

    static bool static_boolean()
    {
        return static_rd() % 2 == 1;
    }

private:
    random_engine rd;

    inline static random_engine static_rd = random_engine((uint)time(0));
};

template<>
inline float Random::number()
{
    return rd() % RANDOM_MAX / (float)(RANDOM_MAX);
}
template<>
inline float Random::number(float max)
{
    return number<float>() * max;
}
template<>
inline float Random::number(float min, float max)
{
    return min + number<float>() * (max - min);
}

template<>
inline float Random::static_number()
{
    return static_rd() % RANDOM_MAX / (float)(RANDOM_MAX);
}
template<>
inline float Random::static_number(float max)
{
    return static_number<float>() * max;
}
template<>
inline float Random::static_number(float min, float max)
{
    return min + static_number<float>() * (max - min);
}