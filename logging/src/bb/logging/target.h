#pragma once

#include <bb/logging/entry.h>

namespace bb
{
    class LogTarget
    {
    public:
	virtual ~LogTarget() = default;

	virtual void process(bb::LogEntry const & entry) = 0;
	
    protected:
	LogTarget() = default;
	
    };
}

