#pragma once

#include <chrono>
#include <string>

#include <ccb/Time.hpp>
#include <ccb/log/LogLevel.hpp>

namespace cloudcastle { namespace infrastructure { namespace log
{
    class ILogTarget
    {
    public:

        virtual void LogMessage(
            const Time& time,
            LogLevel level,
            const std::wstring& source,
            const std::wstring& message) = 0;
    };
} } }
