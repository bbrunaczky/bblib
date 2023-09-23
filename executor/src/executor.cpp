#include <bb/executor/executor.h>
#include <bb/executor/exceptions.h>

namespace bb
{
    std::shared_ptr<Executor> Executor::_instance;
    std::mutex Executor::_instanceMutex;


    Executor & Executor::instance()
    {
	std::unique_lock<std::mutex> lock(_instanceMutex);
	if (!_instance)
	{
	    _instance = std::shared_ptr<Executor>(new Executor);
	}
	return *_instance;
    }


    Executor::~Executor()
    {
	reset();
    }
    
    ThreadPoolPtr Executor::addThreadPool(std::string const & name, uint32_t numOfThreads)
    {
	if (_threadPools.end() != _threadPools.find(name))
	{
	    throw ThreadPoolExists(name);
	}
	auto tp = std::make_shared<ThreadPool>(name, numOfThreads);
	_threadPools[name] = tp;
	return tp;
    }


    ThreadPoolPtr Executor::getThreadPool(std::string const & name)
    {
	auto it = _threadPools.find(name);
	if (_threadPools.end() == it )
	{
	    throw ThreadPoolNotExist(name);
	}
	return it->second;
    }

    void Executor::stop(bool drain)
    {
	for (auto & [name, pool] : _threadPools)
	{
	    pool->stop(drain);
	}
    }


    void Executor::join()
    {
	for (auto & [name, pool] : _threadPools)
	{
	    pool->join();
	}
	_threadPools.clear();
    }

    
    void Executor::reset(bool drain)
    {
	stop(drain);
	join();
    }

}

