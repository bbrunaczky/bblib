#pragma once

#include <string>
#include <functional>

#include <bb/logging/entry.h>
#include <bb/logging/levels.h>
#include <bb/util/stringifier.h>

namespace bb
{

    class Logger
    {
    public:
	// For creating instance
	friend class Logging;
	
	~Logger() = default;

	// Copy ctor and copy assignment operator are intentionally disabled.
	// If they were enabled, the class should maintain a shared state for the _logLevel
	// variable in order to be able to modify its value for all instances at once.
	// That would require mutexing, so it's better (faster) to use Logger & objects.
	Logger(Logger const &) = delete;
	Logger & operator=(Logger const &) = delete;

	Logger(Logger &&);
	Logger & operator=(Logger &&);

	void setLevel(LogLevel level);

	template <typename... Ts>
	void debug(Ts &&... ts);
	template <typename... Ts>
	void info(Ts &&... ts);
	template <typename... Ts>
	void warning(Ts &&... ts);
	template <typename... Ts>
	void error(Ts &&... ts);
	template <typename... Ts>
	void critical(Ts &&... ts);
	
    private:
	Logger(std::string const & name, LogLevel level, std::function<void (LogEntry &&)> recorder);
	void copy(Logger const &);
	void move(Logger &&);

	template <typename... Ts>
	void log(LogLevel severity, Ts &&... ts);

	std::string _name;
	LogLevel _logLevel;
	std::function<void (LogEntry &&)> _recorder;
	bb::BasicStringifier _stringify;
    };

    //

    template <typename... Ts>
    void Logger::log(LogLevel severity, Ts &&... ts)
    {
	if (static_cast<int>(_logLevel) > static_cast<int>(severity))
	{
	    return;
	}
	LogEntry entry{
	    .level = severity,
	    .text = _stringify(std::forward<Ts>(ts)...),
	    .logger = _name,
	    .timePoint = std::chrono::system_clock::now()
	};
	_recorder(std::move(entry));
    }

    template <typename... Ts>
    void Logger::debug(Ts &&... ts)
    {
	log(LogLevel::DEBUG, std::forward<Ts>(ts)...);
    }
    
    template <typename... Ts>
    void Logger::info(Ts &&... ts)
    {
	log(LogLevel::INFO, std::forward<Ts>(ts)...);
    }
    
    template <typename... Ts>
    void Logger::warning(Ts &&... ts)
    {
	log(LogLevel::WARNING, std::forward<Ts>(ts)...);
    }
    
    template <typename... Ts>
    void Logger::error(Ts &&... ts)
    {
	log(LogLevel::ERROR, std::forward<Ts>(ts)...);
    }
    
    template <typename... Ts>
    void Logger::critical(Ts &&... ts)
    {
	log(LogLevel::CRITICAL, std::forward<Ts>(ts)...);
    }
   
}
