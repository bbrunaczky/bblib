#pragma once

#include <thread>

#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/defer.hpp>

namespace bb
{
    
    /*
     * ThreadPool
     * - One io_context object executed by all threads
     * - start(), stop(), join()
     * - Need to be shared_ptr at the end
     * - name it
     */

    class ThreadPool;
    typedef std::shared_ptr<ThreadPool> ThreadPoolPtr;

    
    class ThreadPool
    {
    public:
	template <typename F>
	friend void boost::asio::post(ThreadPool & tp, F && f);
	template <typename F>
	friend void boost::asio::post(bb::ThreadPoolPtr const & tp, F && f);

	template <typename F>
	friend void boost::asio::dispatch(bb::ThreadPool & tp, F && f);
	template <typename F>
	friend void boost::asio::dispatch(bb::ThreadPoolPtr & tp, F && f);

	template <typename F>
	friend void boost::asio::defer(bb::ThreadPool & tp, F && f);
	template <typename F>
	friend void boost::asio::defer(bb::ThreadPoolPtr & tp, F && f);


	
	ThreadPool(std::string const & name, uint32_t numOfThreads);
	~ThreadPool() = default;

	ThreadPool(ThreadPool const &) = delete;
	ThreadPool(ThreadPool &&) = delete;

	ThreadPool & operator=(ThreadPool const &) = delete;
	ThreadPool & operator=(ThreadPool &&) = delete;

	boost::asio::io_context & getCtx();
	
	/*
	 * Prevent addition of new completion handlers.
	 * drain = true: ignore (remove) already filed completion handlers.
	 */
	void stop(bool drain = true);

	/*
	 * Wait until all completion handlers are processed.
	 */
	void join();

    protected:
    private:
	void loop(uint32_t threadNo);
	
	std::string _name;
	uint32_t _numOfThreads;

	boost::asio::io_context _ctx;
	boost::asio::executor_work_guard<boost::asio::io_context::executor_type> _workGuard;
	std::vector<std::thread> _threads;
    };


    inline boost::asio::io_context & ThreadPool::getCtx()
    {
	return _ctx;
    }
    
    
   
}

namespace boost::asio
{
    template <typename F>
    void post(bb::ThreadPool & tp, F && f)
    {
	boost::asio::post(tp._ctx, std::forward<F>(f));
    }
    template <typename F>
    void post(bb::ThreadPoolPtr const & tp, F && f)
    {
	boost::asio::post(tp->_ctx, std::forward<F>(f));
    }

    
    template <typename F>
    void dispatch(bb::ThreadPool & tp, F && f)
    {
	boost::asio::dispatch(tp._ctx, std::forward<F>(f));
    }
    template <typename F>
    void dispatch(bb::ThreadPoolPtr & tp, F && f)
    {
	boost::asio::dispatch(tp->_ctx, std::forward<F>(f));
    }

    
    template <typename F>
    void defer(bb::ThreadPool & tp, F && f)
    {
	boost::asio::defer(tp._ctx, std::forward<F>(f));
    }
    template <typename F>
    void defer(bb::ThreadPoolPtr & tp, F && f)
    {
	boost::asio::defer(tp->_ctx, std::forward<F>(f));
    }
}

	

