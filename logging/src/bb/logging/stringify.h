#pragma once

#include <string>
#include <type_traits>
#include <iostream>
// todo /bb/ move it to a separate library

namespace bb
{
    // T is convertibe to std::string
    template <typename T, typename std::enable_if<std::is_convertible<T, std::string>::value, void>::type * = nullptr>
    std::string toString(T && t)
    {
	return t;
    }
    
    // T is not convertibe to std::string
    template <typename T, typename std::enable_if<false == std::is_convertible<T, std::string>::value, void>::type * = nullptr>
    std::string toString(T && t)
    {
	return std::to_string(std::forward<T>(t));
    };

    template <typename... Ts>
    std::string stringify(Ts &&... ts)
    {
	std::string text;
	((text += toString(std::forward<Ts>(ts))), ...);
	return text;
    }
    
}
