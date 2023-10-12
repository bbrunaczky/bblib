#pragma once

#include <string>
#include <memory>
#include <map>
#include <mutex>
#include <list>
#include <queue>
#include <condition_variable>

#include <bb/logging/logger.h>
#include <bb/logging/entry.h>
#include <bb/logging/levels.h>
#include <bb/logging/target.h>

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

	void addTarget(std::unique_ptr<LogTarget> && target);

	void start();
	void stop();
	void join();

    private:
	Logging();

	void log(LogEntry && entry);
	
	static std::shared_ptr<Logging> _instance;

	std::mutex _dataMutex;
	uint64_t _counter;
	std::queue<LogEntry> _entries;

	std::mutex _loggersMutex;
	LogLevel _logLevel;
	std::map<std::string, Logger> _loggers;

	std::mutex _loopMutex;
	std::list<std::unique_ptr<LogTarget>> _targets;
	bool _loop;
	bool _running;
	std::condition_variable _loopCv;
	bool _loopCvPred;

	std::mutex _joinMutex;
	std::condition_variable _joinCv;
	
    };
}
