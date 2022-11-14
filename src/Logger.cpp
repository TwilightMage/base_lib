#include "../include/base_lib/Logger.h"

#include "../include/base_lib/Path.h"

Logger* Logger::instance_ = nullptr;

void Logger::init(const Path& root_path)
{
    if (instance_) return;

    instance_ = new Logger(root_path);
}

void Logger::new_log_record(ELogLevel level, const String& category, const String& message)
{
    static const char* levelNames[4] = { "Verbose", "Debug", "Warning", "Error" };
    static const char* levelColors[4] = { CONSOLE_WHITE, CONSOLE_CYAN, CONSOLE_YELLOW, CONSOLE_RED };

    instance_->log_stream_mutex_.lock();
    const auto color_in = instance_->colors_enabled_ ? levelColors[static_cast<int>(level)] : "";
    const auto color_out = instance_->colors_enabled_ ? CONSOLE_RESET : "";
    instance_->log_stream_ << color_in << "[" << DateTime::now().to_string().c() << "] [" << levelNames[static_cast<int>(level)] << "] [" << category.c() << "] " << message.c() << color_out << "\n";
    instance_->log_stream_mutex_.unlock();
}

void Logger::set_colors_enabled(bool enabled)
{
    instance_->log_stream_mutex_.lock();
    instance_->colors_enabled_ = enabled;
    instance_->log_stream_mutex_.unlock();
}

Logger::Logger(const Path& root_path)
    : log_stream_(DateTime::now(), root_path)
{
    
}
