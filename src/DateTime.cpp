#include "base_lib/DateTime.h"

#include <chrono>
#include <time.h>

tm to_tm(const DateTime& in_time)
{
	auto t = tm();
	t.tm_year = in_time.year - 1900;
	t.tm_mon = in_time.month - 1;
	t.tm_mday = in_time.day;
	t.tm_hour = in_time.hour;
	t.tm_min = in_time.minute;
	t.tm_sec = in_time.second;

	return t;
}

std::chrono::system_clock::time_point to_time_point(const DateTime& in_time)
{
	auto tm = to_tm(in_time);
	auto result = std::chrono::system_clock::from_time_t(mktime(&tm));
	result += std::chrono::milliseconds(in_time.millisecond);

	return result;
}

TimeInterval::TimeInterval(int days, int hours, int minutes, int seconds, int milliseconds)
	: days(days)
	, hours(hours)
	, minutes(minutes)
	, seconds(seconds)
	, milliseconds(milliseconds)
{
}

float TimeInterval::get_total_hours() const
{
	return get_total_minutes() / 60.0f;
}

float TimeInterval::get_total_minutes() const
{
	return get_total_seconds() / 60.0f;
}

float TimeInterval::get_total_seconds() const
{
	return get_total_milliseconds() / 1000.0f;
}

int TimeInterval::get_total_milliseconds() const
{
	return milliseconds + (seconds + (minutes + (hours * days * 24) * 60) * 60) * 1000;
}

String TimeInterval::to_string() const
{
	String result = String::make(milliseconds) + "ms";
	if (days > 0 || hours > 0 || minutes > 0 || seconds > 0) result = String::make(seconds) + "s, " + result;
	if (days > 0 || hours > 0 || minutes > 0) result = String::make(seconds) + "m, " + result;
	if (days > 0 || hours > 0) result = String::make(seconds) + "h, " + result;
	if (days > 0) result = String::make(seconds) + "d, " + result;
	return result;
}

DateTime::DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond)
	: year(year)
	, month(month)
	, day(day)
	, hour(hour)
	, minute(minute)
	, second(second)
	, millisecond(millisecond)
{
}

DateTime::DateTime()
	: year(0)
	, month(0)
	, day(0)
	, hour(0)
	, minute(0)
	, second(0)
	, millisecond(0)
{
}

String DateTime::to_string(String format) const
{
	auto t = to_tm(*this);

	char time_str_buf[26];
	const uint time_str_buf_size = static_cast<uint>(strftime(time_str_buf, sizeof(time_str_buf), format.c(), &t));
	return String(time_str_buf, time_str_buf_size);
}

DateTime DateTime::now()
{
	time_t time_now = time(0);
	tm* local_time_now = new tm();
	localtime_s(local_time_now, &time_now);

	DateTime result = {
		local_time_now->tm_year + 1900,
		local_time_now->tm_mon + 1,
		local_time_now->tm_mday,
		local_time_now->tm_hour,
		local_time_now->tm_min,
		local_time_now->tm_sec,
		std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 1000
	};
	delete local_time_now;
	return result;
}

TimeInterval DateTime::epoch_time()
{
	return now() - DateTime{ 1970, 1, 1, 0, 0, 0, 0 };
}

DateTime::operator String() const
{
	return to_string();
}

TimeInterval DateTime::operator-(const DateTime& rhs) const
{
	const auto my_point = to_time_point(*this);
	const auto rhs_point = to_time_point(rhs);

	const auto interval = std::chrono::duration_cast<std::chrono::milliseconds>(my_point - rhs_point).count();

	const long long s_div = 1000;
	const long long m_div = s_div * 60;
	const long long h_div = m_div * 60;
	const long long d_div = h_div * 24;

	const int day = static_cast<int>((interval) / d_div);
	const int hour = static_cast<int>((interval - (day * d_div)) / h_div);
	const int minute = static_cast<int>((interval - (day * d_div) - (hour * h_div)) / m_div);
	const int second = static_cast<int>((interval - (day * d_div) - (hour * h_div) - (minute * m_div)) / s_div);
	const int millisecond = static_cast<int>((interval - (day * d_div) - (hour * h_div) - (minute * m_div) - (second * s_div)));

	return { day, hour, minute, second, millisecond };
}
