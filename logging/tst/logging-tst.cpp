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

TEST_F(LoggingTest, basic)
{
    bb::Logging & logging = bb::Logging::instance();
    bb::Logger & logger = logging.logger();
    logging.setLevel(bb::LogLevel::DEBUG);

    // std::cout << std::is_convertible<int, std::string>::value << std::endl;

    std::string str = bb::Stringify("apple", 1, 2);
    std::cout << "'" << bb::Stringify("apple", 1, 2) << "'" << std::endl;
    // std::cout << "'" << bb::Stringify<false>("apple", 1, 2) << "'" << std::endl;

    
    // std::cout << bb::stringify("apple", std::string("apple"), 1, 2.3) << std::endl;
    // std::cout << bb::stringify() << std::endl;
    // std::string str("debug-apple: ");
    // logger.debug(str);
    
    logger.debug("debug-apple: ", 1);
    // logger.info("info-apple");
    // logger.warning("warning-apple");
    // logger.error("error-apple");
    // logger.critical("critical-apple");
    // logger.debug("debug-apple ", "debug-banana");
    // logger.warning("warning-apple", "warning-banana");
    // logger.error("error-apple", "error-banana");
}



int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
