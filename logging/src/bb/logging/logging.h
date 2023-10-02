#pragma once

#include <string>
#include <memory>
#include <map>
#include <mutex>
#include <list>
#include <queue>

#include <bb/logging/logger.h>
#include <bb/logging/entry.h>
#include <bb/logging/levels.h>
#include <bb/logging/consumer.h>

namespace bb
{

    class Logging
    {
    public:

	static Logging & instance();
	~Logging() = default;
	
	Logging(Logging const &) = delete;
	Logging(Logging &&) = delete;
	Logging & operator=(Logging const &) = delete;
	Logging & operator=(Logging &&) = delete;

	void setLevel(LogLevel level);
	
	Logger & logger(std::string const & name = "main");
    private:
	Logging();

	void log(LogEntry && entry);
	
	static std::shared_ptr<Logging> _instance;

	std::mutex _mutex;
	LogLevel _logLevel;
	uint64_t _counter;
	std::map<std::string, Logger> _loggers;
	std::queue<LogEntry> _entries;
	std::list<std::unique_ptr<LogConsumerBase>> _consumers;
    };
}
