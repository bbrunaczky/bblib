#pragma once

namespace bb
{
    struct ThreadPoolException: public std::runtime_error
    {
	using runtime_error::runtime_error;
    };

    
    struct ThreadPoolExists: public ThreadPoolException
    {
	using ThreadPoolException::ThreadPoolException;
    };


    struct ThreadPoolNotExist: public ThreadPoolException
    {
	using ThreadPoolException::ThreadPoolException;
    };
}
