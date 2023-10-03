#pragma once

#include <gtest/gtest.h>
#include <ostream>

#include <bb/util/stringifier.h>


class StringifierTest: public testing::Test
{
protected:
    StringifierTest()
    {}

    
    ~StringifierTest()
    {
    }
};

    
TEST_F(StringifierTest, basic)
{
    bb::BasicStringifier stringify;
    EXPECT_EQ(stringify("apple"), "apple");
    EXPECT_EQ(stringify(std::string("apple")), "apple");
    EXPECT_EQ(stringify(23.45), "23.450000");
    EXPECT_EQ(stringify("apple", 1, 2.3), "apple12.300000");
}
