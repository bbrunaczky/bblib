#include <gtest/gtest.h>
#include <chrono>
#include <iostream>
#include <algorithm>

#include <boost/asio.hpp>

#include <executor/executor.h>
#include <executor/exceptions.h>

using namespace std::chrono_literals;

namespace
{
    namespace pools
    {
	std::string log{"log"};
	std::string test{"test"};
    };
}


class ExecutorTest: public testing::Test
{
protected:
    ExecutorTest():
	_executor(bb::Executor::instance())
    {}

    
    ~ExecutorTest()
    {
	_executor.reset();
    }

    bb::Executor & _executor;
};


TEST_F(ExecutorTest, createDestroy)
{
    auto tp = _executor.addThreadPool(pools::log, 2);
    ASSERT_EQ(tp, _executor.getThreadPool(pools::log));
}


TEST_F(ExecutorTest, doubleCreation)
{
    _executor.addThreadPool(pools::log, 2);
    EXPECT_THROW({
	    _executor.addThreadPool(pools::log, 2);
	},
	bb::ThreadPoolExists);
}

TEST_F(ExecutorTest, invoke)
{
    int counter = 0;
    std::mutex mutex;
    std::condition_variable cv;
    
    auto tp = _executor.addThreadPool(pools::test, 2);

    std::unique_lock<std::mutex> lock(mutex);
    auto lambda = [&mutex, &counter, &cv] ()
		  {
		      std::unique_lock<std::mutex> lock(mutex);
		      ++counter;
		      cv.notify_one();
		  };
    
    boost::asio::post(*tp, lambda);
    boost::asio::post(tp, lambda);
    boost::asio::dispatch(*tp, lambda);
    boost::asio::dispatch(tp, lambda);
    boost::asio::defer(*tp, lambda);
    boost::asio::defer(tp, lambda);

    auto ret = cv.wait_for(lock,
			   2s,
			   [&counter] ()
			   {
			       return 6 == counter;
			   });
    EXPECT_TRUE(ret);
    EXPECT_EQ(counter, 6);
}


TEST_F(ExecutorTest, stopJoinWithDrain)
{
    /*
     * Scenario:
     * - Create threadpool with 1 thread
     * - Start a timer
     * - Add the 2 functions to the queue
     * - Stop the threadpool once the first function started and make sure the first function keeps running
     *   until the threadpool is stopped to prevent early-process of the 2nd and 3rd functions.
     * - Add the 3rd function to the queue.
     *
     * Expectation:
     * - Only the first function ran.
     * - The 2nd and 3rd functions weren't executed as the threadpool had been stopped with drain=true before their turn.
     * - The timer hasn't gone off.
     */
    int counter = 0;;
    std::mutex mutex;

    std::condition_variable cv;
    int cvPred = 0;
    
    auto tp = _executor.addThreadPool(pools::test, 1);

    boost::asio::deadline_timer t(tp->getCtx());
    t.expires_from_now(boost::posix_time::seconds(5));
    t.async_wait([] (boost::system::error_code)
		 {
		     std::cout << "timer expired" << std::endl;
		     throw std::runtime_error("We are not supposed to be here, reset() should have cancelled this timer.");
		 });
    
    auto firstFunc =
	[&cv, &mutex, &counter, &cvPred] ()
	{
	    std::stringstream sstr;
	    sstr << "[" << std::this_thread::get_id() << "] ";
	    std::string threadId = sstr.str();

	    // notify the main thread that we started
	    std::unique_lock<std::mutex> lock(mutex);
	    ++cvPred;
	    cv.notify_one();

	    // Wait for Executor::stop(). If we don't wait here the stop(), the increaseFunc might be loaded and executed.
	    auto ret = cv.wait_for(lock, 2s, [&cvPred] () { return 2 == cvPred; });
	    ASSERT_TRUE(ret);

	    ++counter;
	};

    auto increaseFunc = 
	[&mutex, &counter] ()
	{
	    std::unique_lock<std::mutex> lock(mutex);
	    ++counter;
	};
	
    {
	// Important: the lock need to be released after the notify_one() at the bottom of this scope.
	// If it's not released, the wait() in the threadpool can't reacquire the mutex, therefore it's going to block forever.
	std::unique_lock<std::mutex> lock(mutex);
	boost::asio::post(tp, firstFunc);
	boost::asio::post(tp, increaseFunc); // Adding to the queue, though it's not supposed to be executed due to Executor::stop(drain=true)

	// Make sure the firstFunc is under way
	auto ret = cv.wait_for(lock, 2s, [&cvPred] () { return 1 == cvPred; });
	ASSERT_TRUE(ret);

	// Stop the executor and notify the first function.
	_executor.stop();
	++cvPred;
	cv.notify_one();
    }

    // Add the 3rd function
    boost::asio::post(tp, increaseFunc);
    _executor.join();
    
    ASSERT_EQ(counter, 1);
}



TEST_F(ExecutorTest, stopJoinWithoutDrain)
{
    /*
     * Scenario:
     * - Create threadpool with 1 thread
     * - Add the 3 functions to the queue
     * - Stop the threadpool with drain=false once the first function started and make sure the first
     *   function keeps running until the threadpool is stopped to prevent early-process of the 2nd and 3rd function.
     *
     * Expectation:
     * - All the 3 functions were executed due to drain=false
     */
    int counter = 0;;
    std::mutex mutex;

    std::condition_variable cv;
    int cvPred = 0;
    
    auto tp = _executor.addThreadPool(pools::test, 1);
    
    auto firstFunc =
	[&cv, &mutex, &counter, &cvPred] ()
	{
	    std::stringstream sstr;
	    sstr << "[" << std::this_thread::get_id() << "] ";
	    std::string threadId = sstr.str();

	    // notify the main thread that we started
	    std::unique_lock<std::mutex> lock(mutex);
	    ++cvPred;
	    cv.notify_one();

	    // Wait for Executor::stop(). If we don't wait here the stop(), the increaseFunc might be loaded and executed.
	    auto ret = cv.wait_for(lock, 2s, [&cvPred] () { return 2 == cvPred; });
	    ASSERT_TRUE(ret);

	    ++counter;
	};

    auto increaseFunc = 
	[&mutex, &counter] ()
	{
	    std::unique_lock<std::mutex> lock(mutex);
	    ++counter;
	};
	
    {
	// Important: the lock need to be released after the notify_one() at the bottom of this scope.
	// If it's not released, the wait() in the threadpool can't reacquire the mutex, therefore it's going to block forever.
	std::unique_lock<std::mutex> lock(mutex);
	boost::asio::post(tp, firstFunc);
	boost::asio::post(tp, increaseFunc);

	// Make sure the firstFunc is under way
	auto ret = cv.wait_for(lock, 2s, [&cvPred] () { return 1 == cvPred; });
	ASSERT_TRUE(ret);

	// Stop the executor and notify the first function.
	_executor.stop(false);
	++cvPred;
	cv.notify_one();
    }

    boost::asio::post(tp, increaseFunc);
    _executor.join();
    
    ASSERT_EQ(counter, 3);
}


TEST_F(ExecutorTest, enableThreadPool)
{
    auto tp = _executor.addThreadPool(pools::test, 2);
    _executor.addThreadPool(pools::log, 2);

    struct Task: private bb::EnableThreadPool
    {
	std::vector<std::thread::id> _tpThreadIds;
	
	Task(bb::ThreadPoolPtr tp):
	    EnableThreadPool(tp)
	{
	    _tpThreadIds = tp->getThreadIds();
	}

	
	void proceed()
	{
	    auto lambda = [this] ()
			  {
			      std::thread::id currentThreadId = std::this_thread::get_id();
			      auto const it = std::find(_tpThreadIds.cbegin(), _tpThreadIds.cend(), currentThreadId);
			      ASSERT_NE(_tpThreadIds.end(), it) << "Couldn't find current thread id in the threadpool!";
			  };
	    post(lambda);
	    dispatch(lambda);
	    defer(lambda);
	}
    };

    Task task(tp);
    task.proceed();

    _executor.stop(false);
    _executor.join();
    
}


int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
