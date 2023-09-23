#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <mutex>
#include <map>

#include <executor/threadpool.h>


namespace bb
{

    class Executor
    {
    public:

	static Executor & instance();
	~Executor();
	
	Executor(Executor const &) = delete;
	Executor(Executor &&) = delete;

	Executor & operator=(Executor const &) = delete;
	Executor & operator=(Executor &&) = delete;
	
	ThreadPoolPtr addThreadPool(std::string const & name, uint32_t numOfThreads);
	ThreadPoolPtr getThreadPool(std::string const & name);

	/*
	 * drain = true: don't process already- and future-submitted completion handlers.
	 * drain = false: stops the threadpool's loop once the queue empty.
	 */
	void stop(bool drain = true);
	void join();

	/*
	 * stop(drain) + join()
	 */
	void reset(bool drain = true);

    private:
	Executor() = default;

	static std::shared_ptr<Executor> _instance;
	static std::mutex _instanceMutex;

	std::map<std::string, ThreadPoolPtr> _threadPools;
    };
    
    // void init();
}
