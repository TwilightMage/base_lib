#include "../include/Logger.h"

Logger* Logger::instance_ = nullptr;

void Logger::init(const String& root_path)
{
    if (instance_) return;

    instance_ = new Logger(root_path);
}

void Logger::new_log_record(ELogLevel level, const String& category, const String& message)
{
    static const char* levelNames[4] = { "Verbose", "Debug", "Warning", "Error" };
    static const char* levelColors[4] = { CONSOLE_WHITE, CONSOLE_CYAN, CONSOLE_YELLOW, CONSOLE_RED };

    instance_->log_stream_mutex_.lock();
    instance_->log_stream_ << levelColors[static_cast<int>(level)] << "[" << DateTime::now().to_string().c() << "] [" << levelNames[static_cast<int>(level)] << "] [" << category.c() << "] " << message.c() << CONSOLE_RESET << "\n";
    instance_->log_stream_mutex_.unlock();
}

Logger::Logger(const String& root_path)
    : log_stream_(DateTime::now(), root_path)
{
    
}
