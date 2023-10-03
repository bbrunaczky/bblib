#pragma once

#include <string>
#include <type_traits>
#include <ostream>
// todo /bb/ move it to a separate library

namespace bb
{

    /* todos
     * stringify<space = true> -- default: false
     * stringify<precision = N> -- default: 4
     * cpp-guide:
     *     - fold expressions
     *     - is_convertible<>
     *     - enable_if<>
     */

    // todo /bb/ move to detail namespace
    
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
	((text += ' ' + toString(std::forward<Ts>(ts))), ...);
    }
    
    template <>
    template <typename T, typename... Ts>
    Stringify<false>::Stringify(T && t, Ts &&... ts)
    {
	text = t;
	((text += toString(std::forward<Ts>(ts))), ...);
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


    template <bool space>
    std::ostream & operator<<(std::ostream & out, Stringify<space> && sfy)
    {
	out << sfy.str();
	return out;
    }
    
}
