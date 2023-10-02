#include <bb/logging/logging.h>

#include <mutex>
#include <stdexcept>
#include <functional>
#include <iostream>

namespace bb
{
    std::shared_ptr<Logging> Logging::_instance;


    Logging & Logging::instance()
    {
	static std::once_flag flag;
	std::call_once(flag,
		       [] ()
		       {
			   bb::Logging::_instance.reset(new Logging());
		       });
	return *_instance;
    }


    Logging::Logging():
	_logLevel(LogLevel::INFO),
	_counter(0)
    {}

    
    void Logging::setLevel(LogLevel level)
    {
	std::unique_lock<std::mutex> lock(_mutex);
	_logLevel = level;
	for (auto & [k, v] : _loggers)
	{
	    v.setLevel(_logLevel);
	}
    }
    
    Logger & Logging::logger(std::string const & name)
    {
	std::unique_lock<std::mutex> lock(_mutex);
	decltype(_loggers)::iterator it;
	it = _loggers.find(name);
	if (_loggers.end() == it)
	{
	    // Piecewise construct would be better, but it requires public Logger constructor.
	    auto [newIt, inserted] = _loggers.emplace(std::make_pair(name, Logger(name, _logLevel, std::bind(&Logging::log, this, std::placeholders::_1))));
	    if (!inserted)
	    {
		throw std::runtime_error("Couldn't insert new logger!");
	    }
	    it = newIt;
	}
	return it->second;
    }

    void Logging::log(LogEntry && entry)
    {
	std::unique_lock<std::mutex> lock(_mutex);
	entry.counter = _counter++;
	std::cout << entry.counter << "\t"
		  << static_cast<int>(entry.level) << "\t"
		  << entry.text << "\t"
		  << entry.logger << "\t"
		  << entry.timePoint.time_since_epoch().count() << std::endl;
	_entries.push(std::move(entry));
    }

    /*
     * Implement addConsumer() function.
     * Create a consume() function that can be run from a separate thread.
     * It checks the queue, process the elements as long there's any.
     *   - lock
     *   - front()
     *   - pop()
     *   - release
     *   - process
     * Wait for a new element via condition_variable.
     */

}


