#pragma once

#include <fstream>

#include "DateTime.h"
#include "TeeBuf.h"

#define CONSOLE_RESET        "\033[0m"
#define CONSOLE_BLACK        "\033[30m"        // Black
#define CONSOLE_RED          "\033[31m"        // Red
#define CONSOLE_GREEN        "\033[32m"        // Green
#define CONSOLE_YELLOW       "\033[33m"        // Yellow
#define CONSOLE_BLUE         "\033[34m"        // Blue
#define CONSOLE_MAGENTA      "\033[35m"        // Magenta
#define CONSOLE_CYAN         "\033[36m"        // Cyan
#define CONSOLE_WHITE        "\033[37m"        // White
#define CONSOLE_BOLD_BLACK   "\033[1m\033[30m" // Bold Black
#define CONSOLE_BOLD_RED     "\033[1m\033[31m" // Bold Red
#define CONSOLE_BOLD_GREEN   "\033[1m\033[32m" // Bold Green
#define CONSOLE_BOLD_YELLOW  "\033[1m\033[33m" // Bold Yellow
#define CONSOLE_BOLD_BLUE    "\033[1m\033[34m" // Bold Blue
#define CONSOLE_BOLD_MAGENTA "\033[1m\033[35m" // Bold Magenta
#define CONSOLE_BOLD_CYAN    "\033[1m\033[36m" // Bold Cyan
#define CONSOLE_BOLD_WHITE   "\033[1m\033[37m" // Bold White

class EXPORT LogStream : public std::ostream
{
public:
    explicit LogStream(const DateTime& log_start_time, const String& root);
	~LogStream() override;

private:
	std::ofstream latest_log_file_;
	std::ofstream timed_log_file_;
	TeeBuf tbuf_;
};
