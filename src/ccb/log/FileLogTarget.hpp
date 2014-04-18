#pragma once

#include <chrono>
#include <fstream>
#include <string>

#include <ccb/log/ILogTarget.hpp>

namespace ccb { namespace log
{
    class FileLogTarget : public ILogTarget
    {
    private:

        std::string fileName;

    public:

        FileLogTarget(const std::wstring& fileName)
            : fileName(fileName.begin(), fileName.end())
        {
        }

    public:

        virtual void LogMessage(
            const Time& time,
            LogLevel level,
            const std::wstring& source,
            const std::wstring& message) override
        {
            std::wofstream stream(this->fileName, std::ios_base::out | std::ios_base::app);
            stream
                << time << L" : "
                << level << L" : "
                << source << L" : "
                << message << std::endl;
        }
    };
} } }
