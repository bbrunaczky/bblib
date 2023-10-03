#include <bb/logging/logger.h>

namespace bb
{

    Logger::Logger(std::string const & name, LogLevel level, std::function<void (LogEntry &&)> recorder):
	_name(name),
	_logLevel(level),
	_recorder(std::move(recorder))
    {}

    
    Logger::Logger(Logger && rhs)
    {
	move(std::move(rhs));
    }


    Logger & Logger::operator=(Logger && rhs)
    {
	if (this != &rhs)
	{
	    move(std::move(rhs));
	}
    	return *this;
    }

    
    void Logger::move(Logger && rhs)
    {
	_name = std::move(rhs._name);
	_logLevel = rhs._logLevel;
	_recorder = std::move(rhs._recorder);
    }


    void Logger::setLevel(LogLevel level)
    {
	_logLevel = level;
    }

    
}
