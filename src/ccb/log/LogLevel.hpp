#pragma once

#include <ostream>

namespace cloudcastle { namespace infrastructure { namespace log
{
    enum class LogLevel
    {
        Trace = 0,

        Log = 1,

        Info = 2,

        Warning = 3,

        Error = 4
    };

    std::wostream& operator << (std::wostream& stream, LogLevel level)
    {
        switch (level)
        {
        case LogLevel::Trace:
            stream << L"Trace";
            break;

        case LogLevel::Log:
            stream << L"Log";
            break;

        case LogLevel::Info:
            stream << L"Info";
            break;

        case LogLevel::Warning:
            stream << L"Warning";
            break;

        case LogLevel::Error:
            stream << L"Error";
            break;

        default:
            stream << L"Unknown";
        };

        return stream;
    }
} } }
