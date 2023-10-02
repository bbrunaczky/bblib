#pragma once

#include <string>
#include <functional>

#include <bb/logging/entry.h>
#include <bb/logging/levels.h>
#include <bb/logging/stringify.h>

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
	
	void info(std::string const & text);
	void warning(std::string const & text);
	void error(std::string const & text);
	void critical(std::string const & text);
	
    private:
	Logger(std::string const & name, LogLevel level, std::function<void (LogEntry &&)> recorder);
	void copy(Logger const &);
	void move(Logger &&);

	template <typename... Ts>
	void log(LogLevel severity, Ts &&... ts);

	std::string _name;
	LogLevel _logLevel;
	std::function<void (LogEntry &&)> _recorder;
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
	    .text = bb::stringify(std::forward<Ts>(ts)...),
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
   
    
    
}
