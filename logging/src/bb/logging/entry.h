#pragma once

#include <string>
#include <chrono>

#include <bb/logging/levels.h>

namespace bb
{
    struct LogEntry
    {
	uint64_t counter;
	LogLevel level;
	std::string text;
	std::string logger;
	std::chrono::time_point<std::chrono::system_clock> timePoint;
    };
}
