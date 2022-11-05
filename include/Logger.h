#pragma once

#include <mutex>

#include "LogStream.h"
#include "String.h"

enum class ELogLevel
{
    Verbose,
    Debug,
    Warning,
    Error
};

class EXPORT Logger
{
public:
    static void init(const String& root_path);
    
    static void new_log_record(ELogLevel level, const String& category, const String& message);

private:
    Logger(const String& root_path);
    
    LogStream log_stream_;
    std::mutex log_stream_mutex_;

    static Logger* instance_;
};

template<typename... Args>
static void print_debug(const String& category, const String& format, Args... args)
{
    const int size = snprintf(nullptr, 0, format.c(), std::forward<Args>(args)...);
    char* buffer = new char[size + 1];
    sprintf_s(buffer, size + 1, format.c(), std::forward<Args>(args)...);

    Logger::new_log_record(ELogLevel::Debug, category, buffer);
}

template<typename... Args>
static void print_warning(const String& category, const String& format, Args... args)
{
    const int size = snprintf(nullptr, 0, format.c(), std::forward<Args>(args)...);
    char* buffer = new char[size + 1];
    sprintf_s(buffer, size + 1, format.c(), std::forward<Args>(args)...);

    Logger::new_log_record(ELogLevel::Warning, category, buffer);
}

template<typename... Args>
static void print_error(const String& category, const String& format, Args... args)
{
    const int size = snprintf(nullptr, 0, format.c(), std::forward<Args>(args)...);
    char* buffer = new char[size + 1];
    sprintf_s(buffer, size + 1, format.c(), std::forward<Args>(args)...);

    Logger::new_log_record(ELogLevel::Error, category, buffer);
}

template<typename... Args>
static void verbose(const String& category, const String& format, Args... args)
{
    const int size = snprintf(nullptr, 0, format.c(), std::forward<Args>(args)...);
    char* buffer = new char[size + 1];
    sprintf_s(buffer, size + 1, format.c(), std::forward<Args>(args)...);

    Logger::new_log_record(ELogLevel::Verbose, category, buffer);
}
