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



    // bb::Logger & logger = logging.logger();
    // logging.setLevel(bb::LogLevel::DEBUG);

    // struct Target: public bb::LogTarget
    // {
    // 	void process(bb::LogEntry const & entry) override
    // 	{
    // 	}
    // };

    // std::unique_ptr<Target> target{new Target()};
    // logging.addTarget(std::move(target));

    // logger.debug("debug-apple: ", 1);
    // logger.info("info-apple");
    // logger.warning("warning-apple");
    // logger.error("error-apple");
    // logger.critical("critical-apple");
    // logger.debug("debug-apple ", "debug-banana");
    // logger.warning("warning-apple ", "warning-banana");
    // logger.error("error-apple ", "error-banana");


int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
