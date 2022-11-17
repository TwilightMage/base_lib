#include "base_lib/LogStream.h"

#include <iostream>

#include "base_lib/Path.h"

LogStream::LogStream(const DateTime& log_start_time, const Path& root)
	: std::ostream(&tbuf_)
{
	auto logs = root.up().get_child("logs");

	logs.create_dir();
	timed_log_file_ = std::ofstream(logs.get_child(log_start_time.to_string("%Y_%m_%d_%H_%M_%S") + ".log").to_string().c());
	latest_log_file_ = std::ofstream(logs.get_child("latest.log").to_string().c());

	auto a = timed_log_file_.is_open();
	auto b = latest_log_file_.is_open();

	tbuf_ = TeeBuf({ std::cout.rdbuf(), timed_log_file_.rdbuf(), latest_log_file_.rdbuf() });
}

LogStream::~LogStream()
{
	timed_log_file_ << "\n";
	timed_log_file_.close();

	latest_log_file_ << "\n";
	latest_log_file_.close();
}