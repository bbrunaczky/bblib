#pragma once

#include <gtest/gtest.h>
#include <bb/util/stringify.h>


class StringifyTest: public testing::Test
{
protected:
    StringifyTest()
    {}

    
    ~StringifyTest()
    {
    }
};


TEST_F(StringifyTest, noSpace)
{
    {
	std::string str = bb::Stringify("apple");
	EXPECT_EQ(str, "apple");
    }
    
    {
	std::string str = bb::Stringify("apple", 1, 2.3);
	EXPECT_EQ(str, "apple12.300000");
    }
}


TEST_F(StringifyTest, space)
{
    {
	std::string str = bb::Stringify<true>("apple");
	EXPECT_EQ(str, "apple");
    }
    
    {
	std::string str = bb::Stringify<true>("apple", 1);
	EXPECT_EQ(str, "apple 1");
    }

    
    // std::cout << "'" << bb::Stringify("apple", 1, 2) << "'" << std::endl;
    // std::cout << "'" << bb::Stringify<false>("apple", 1, 2) << "'" << std::endl;

    
    // std::cout << bb::stringify("apple", std::string("apple"), 1, 2.3) << std::endl;
    // std::cout << bb::stringify() << std::endl;

}
