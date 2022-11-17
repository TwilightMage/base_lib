#pragma once

#include <chrono>

#include "Map.h"

typedef std::chrono::duration<long long, std::ratio<1, 1000000000>> dur_nano;
typedef std::chrono::time_point<std::chrono::system_clock> time_point;

template<typename... TimePointTypes>
void measure_time_all_impl(List<dur_nano>& intervals, TimePointTypes... time_points)
{
    List<time_point> time_points_list = {time_points...};
    for (uint i = 0; i < sizeof...(time_points) - 1; i++)
    {
        intervals.add(time_points_list[i + 1] - time_points_list[i]);
    }
}

void measure_time_markers_impl(List<dur_nano>& intervals, List<time_point> time_points, uint& time_total)
{
    for (uint i = 0; i < time_points.length() - 1; i++)
    {
        const dur_nano time = time_points[i + 1] - time_points[i];
        intervals.add(time);
        time_total += (uint)time.count();
    }
}

#define get_now(name) const time_point name = std::chrono::system_clock::now();
#define measure_time(name, a, b) const dur_nano name = b - a;
#define measure_now(name, a) const dur_nano name = std::chrono::system_clock::now() - a;
#define measure_time_all(name, ...) List<dur_nano> name; measure_time_all_impl(name, __VA_ARGS__);
#define get_now_prepare_markers(level) List<time_point> time_points_##level;
#define get_now_marker(level) time_points_##level.add(std::chrono::system_clock::now());
#define measure_time_markers(level) List<dur_nano> time_intervals_##level; uint time_total_##level = 0; measure_time_markers_impl(time_intervals_##level, time_points_##level, time_total_##level);
    
