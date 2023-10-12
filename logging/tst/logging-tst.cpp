#include <gtest/gtest.h>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <thread>

#include <boost/asio.hpp>

#include <bb/logging/logging.h>

using namespace std::chrono_literals;

namespace
{
}


class LoggingTest: public testing::Test
{
protected:
    LoggingTest()
    {}

    
    ~LoggingTest()
    {
    }
};


TEST_F(LoggingTest, instance)
{
    bb::Logging & logging = bb::Logging::instance();
    bb::Logger & logger = logging.logger();

    {
	bb::Logger & logger2 = logging.logger();
	EXPECT_EQ(&logger, &logger2);
    }
    {
	bb::Logger & logger2 = logging.logger("dummy-module");
	EXPECT_NE(&logger, &logger2);
    }
}


TEST_F(LoggingTest, startStopEarlyJoin)
{
    bb::Logging & logging = bb::Logging::instance();
    
    std::thread t1([&] () { logging.start(); });
    std::thread t2([&] ()
		   {
		       std::this_thread::sleep_for(2s);
		       logging.stop();
		   });
    std::this_thread::sleep_for(1s);
    logging.join();
    
    t1.join();
    t2.join();
}


TEST_F(LoggingTest, startStopLateJoin)
{
    bb::Logging & logging = bb::Logging::instance();
    
    std::thread t1([&] () { logging.start(); });
    std::this_thread::sleep_for(1s);
    logging.stop();
    logging.join();
    
    t1.join();
}



TEST_F(LoggingTest, target)
{
    bb::Logging & logging = bb::Logging::instance();
    bb::Logger & logger = logging.logger();
    logging.setLevel(bb::LogLevel::DEBUG);
    std::thread t1([&] () { logging.start(); });

    struct Target: public bb::LogTarget
    {
	void process(bb::LogEntry const & entry) override
	{
	    // todo /bb/ check google mocks
	    std::cout << entry.counter << "\t"
		      << static_cast<int>(entry.level) << "\t"
		      << entry.text << "\t"
		      << entry.logger << "\t"
		      << entry.timePoint.time_since_epoch().count() << std::endl;
	}
    };

    logging.addTarget(std::make_unique<Target>());

    logger.debug("debug-apple: ", 1);
    logger.info("info-apple");
    logger.warning("warning-apple");
    logger.error("error-apple");
    logger.critical("critical-apple");
    logger.debug("debug-apple ", "debug-banana");
    logger.warning("warning-apple ", "warning-banana");
    logger.error("error-apple ", "error-banana");

    std::this_thread::sleep_for(1s);
    logging.stop();
    logging.join();
    
    t1.join();

}



int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
