#include <executor/threadpool.h>

namespace bb
{
    ThreadPool::ThreadPool(std::string const & name, uint32_t numOfThreads):
	_name(name),
	_numOfThreads(numOfThreads),
	_workGuard(_ctx.get_executor())
    {
	for (uint32_t i=0; i<_numOfThreads; ++i)
	{
	    _threads.emplace_back(std::bind(&ThreadPool::loop, this, i));
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

    
    void ThreadPool::loop(uint32_t threadNo)
    {
	_ctx.run();
    }


    
}
