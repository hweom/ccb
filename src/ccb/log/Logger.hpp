// The MIT License (MIT)
//
// Copyright (c) 2014 Mikhail Balakhno
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
