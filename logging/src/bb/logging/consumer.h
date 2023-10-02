#pragma once

#include <bb/logging/entry.h>

namespace bb
{
    class LogConsumerBase
    {
    public:
	virtual ~LogConsumerBase() = default;

	virtual void signal(LogEntry const & entry) = 0;
	
    protected:
	LogConsumerBase() = default;
	
    };
}

