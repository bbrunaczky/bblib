#pragma once

#include <string>
#include <type_traits>
#include <ostream>

namespace bb
{

    namespace detail
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
    }

    
    template <bool space = false>
    struct Stringify
    {
	std::string text;

	template <typename T, typename... Ts>
	Stringify(T && t, Ts &&... ts);

	std::string str() const;

	operator std::string() const;
    };


    template <>
    template <typename T, typename... Ts>
    Stringify<true>::Stringify(T && t, Ts &&... ts)
    {
	text = t;
	((text += ' ' + detail::toString(std::forward<Ts>(ts))), ...);
    }
    
    template <>
    template <typename T, typename... Ts>
    Stringify<false>::Stringify(T && t, Ts &&... ts)
    {
	text = t;
	((text += detail::toString(std::forward<Ts>(ts))), ...);
    }
    
    template <bool space>
    std::string Stringify<space>::str() const
    {
	return text;
    }

    template <bool space>
    Stringify<space>::operator std::string() const
    {
	return text;
    }


    template <typename T, bool space>
    T & operator<<(T && out, Stringify<space> && sfy) // Allow both lvalue and rvalue out objects, allow only rvalue Stringify.
    {
	out << sfy.str();
	return out;
    }
    
}