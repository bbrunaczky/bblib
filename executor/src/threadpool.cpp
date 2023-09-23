#include <executor/threadpool.h>
#include <chrono>

using namespace std::chrono_literals;

namespace bb
{
    ThreadPool::ThreadPool(std::string const & name, uint32_t numOfThreads):
	_name(name),
	_numOfThreads(numOfThreads),
	_workGuard(_ctx.get_executor())
    {
	_threadIds.reserve(numOfThreads);
	std::unique_lock<std::mutex> lock(_mutex);
	for (uint32_t i=0; i<_numOfThreads; ++i)
	{
	    _threads.emplace_back(std::bind(&ThreadPool::loop, this, i));
	}
	// Make sure all threads have started
	bool ret = _cv.wait_for(lock,
				2s,
				[this] () -> bool
				{
				    return _numOfThreads == _threadIds.size();
				});
	if (!ret)
	{
	    std::string text = "Threads haven't woken up in 2 seconds. ThreadPool: " + _name + ". Aborting.";
	    throw std::runtime_error(text);
	}
    }


    void ThreadPool::stop(bool drain)
    {
	// Prevent adding new completion handlers to the ctx's queue.
	_workGuard.reset();
	
	if (drain)
	{
	    // Don't process already filed completion handlers from ctx's queue.
	    _ctx.stop();
	}
    }

    
    void ThreadPool::join()
    {
	for (auto & thread : _threads)
	{
	    thread.join();
	}
    }


    std::vector<std::thread::id> ThreadPool::getThreadIds() const
    {
	return _threadIds;
    }
    
    void ThreadPool::loop(uint32_t threadNo)
    {
	{
	    std::unique_lock<std::mutex> lock(_mutex);
	    _threadIds.push_back(std::this_thread::get_id());
	    _cv.notify_one();
	}
	_ctx.run();
    }



    EnableThreadPool::EnableThreadPool(ThreadPoolPtr tp):
	_tp(tp)
    {}
    
}
