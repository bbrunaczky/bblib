#pragma once

#include <string>
#include <type_traits>
#include <ostream>

namespace bb
{

    namespace detail
    {

	// T is convertible to std::string
	template <typename T, typename std::enable_if<std::is_convertible<T, std::string>::value, void>::type * = nullptr>
	std::string toString(T && t)
	{
	    return t;
	}
    
	// T is not convertible to std::string
	template <typename T, typename std::enable_if<false == std::is_convertible<T, std::string>::value, void>::type * = nullptr>
	std::string toString(T && t)
	{
	    return std::to_string(std::forward<T>(t));
	};
    }

    
    struct BasicStringifier
    {
	template <typename T, typename... Ts>
	std::string operator()(T && t, Ts &&... ts)
	{
	    return ( t + ... + detail::toString(std::forward<Ts>(ts)));
	}
    };
    
}
