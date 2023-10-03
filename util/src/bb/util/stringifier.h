#pragma once

#include <string>
#include <type_traits>

namespace bb
{

    namespace detail
    {

	// T is convertible to std::string
	template <typename T,
		  typename std::enable_if<std::is_convertible<T, std::string>::value, void>::type * = nullptr>
	std::string toString(T && t)
	{
	    return t;
	}
    
	// default toString()
	template <typename T,
		  typename std::enable_if<false == std::is_convertible<T, std::string>::value, void>::type * = nullptr>
	std::string toString(T && t)
	{
	    return std::to_string(std::forward<T>(t));
	};
    }

    
    struct BasicStringifier
    {
	template <typename T, typename... Ts>
	std::string operator()(T && t, Ts &&... ts);
	
    private:
    };

    template <typename T, typename... Ts>
    std::string BasicStringifier::operator()(T && t, Ts &&... ts)
    {
	return ( detail::toString(std::forward<T>(t)) + ... + detail::toString(std::forward<Ts>(ts)));
    }
    
}
