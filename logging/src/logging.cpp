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
	_counter(0),
	_logLevel(LogLevel::INFO),
	_loop(false),
	_running(false),
	_loopCvPred(false)
    {}

    
    void Logging::setLevel(LogLevel level)
    {
	std::unique_lock<std::mutex> lock(_loggersMutex);
	_logLevel = level;
	for (auto & [k, v] : _loggers)
	{
	    v.setLevel(_logLevel);
	}
    }

    
    Logger & Logging::logger(std::string const & name)
    {
	std::unique_lock<std::mutex> lock(_loggersMutex);
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
	std::unique_lock<std::mutex> lock(_dataMutex);
	entry.counter = _counter++;
	_entries.push_back(std::move(entry));

	std::unique_lock loopLock(_loopMutex);
	_loopCv.notify_one();
    }

    
    void Logging::addTarget(std::unique_ptr<LogTarget> && target)
    {
	std::unique_lock<std::mutex> lock(_loopMutex);
	_targets.push_back(std::move(target));
	
    }


    void Logging::start()
    {
	std::unique_lock lock(_loopMutex);
	if (_loop)
	{
	    return;
	}
	   
	_loop = true;
	_running = true;
	while (_loop)
	{
	    // todo /bb/ process log entries
	    decltype(_entries) entries;
	    {
		std::unique_lock<std::mutex> dataLock(_dataMutex);
		entries = std::move(_entries);
	    }
	    for (auto const & entry : entries)
	    {
		for (auto & target: _targets)
		{
		    target->process(entry);
		}
	    }
	    
	    _loopCv.wait(lock, [this] () { return _loopCvPred; } );
	    _loopCvPred = false;
    	}
	
	std::unique_lock<std::mutex> joinLock(_joinMutex);
	_running = false;
	_joinCv.notify_one();
    }

    
    void Logging::stop()
    {
	std::unique_lock<std::mutex> lock(_loopMutex);
	_loop = false;
	_loopCvPred = true;
	_loopCv.notify_one();
    }

    
    void Logging::join()
    {
	std::unique_lock<std::mutex> lock(_joinMutex);
	if (!_running)
	{
	    return;
	}
	
	_joinCv.wait(lock, [this] () { return !_running; });
	
    }

}


