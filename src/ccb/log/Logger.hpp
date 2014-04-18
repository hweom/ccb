#pragma once

#include <sstream>

#include <cloudcastle/infrastructure/log/LogSink.hpp>

namespace cloudcastle { namespace infrastructure { namespace log
{
    class Logger
    {
    private:

        LogSink* sink;

        std::wstring name;

    public:

        Logger(const std::wstring& name)
            : sink(&LogSink::GetSink())
            , name(name)
        {
        }

    public:

        template<typename... Params>
        void Write(LogLevel level, Params... params)
        {
            std::wostringstream stream;
            this->Write(stream, level, params...);
            this->sink->WriteMessage(level, this->name, stream.str());
        }

        template<typename... Params>
        void Trace(Params... params)
        {
            this->Write(LogLevel::Trace, params...);
        }

        template<typename... Params>
        void Log(Params... params)
        {
            this->Write(LogLevel::Log, params...);
        }

        template<typename... Params>
        void Info(Params... params)
        {
            this->Write(LogLevel::Info, params...);
        }

        template<typename... Params>
        void Warn(Params... params)
        {
            this->Write(LogLevel::Warning, params...);
        }

        template<typename... Params>
        void Error(Params... params)
        {
            this->Write(LogLevel::Error, params...);
        }
    private:

        template<typename Arg0, typename... Params>
        void Write(std::wostream& stream, LogLevel level, Arg0 arg0, Params... params)
        {
            stream << arg0;

            this->Write(stream, level, params...);
        }

        void Write(std::wostream& stream, LogLevel level)
        {
        }
    };
} } }
