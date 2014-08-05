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

#include <ccb/log/IsStreamable.hpp>
#include <ccb/log/LogSink.hpp>

namespace ccb { namespace log
{
    class Logger
    {
    public:

        template<typename T>
        struct PreferWide
        {
            static const bool value = is_streamable<std::wostream, T>::value;
        };

        template<typename T>
        struct PreferShort
        {
            static const bool value = !is_streamable<std::wostream, T>::value && is_streamable<std::ostream, T>::value;
        };

    private:

        static const wchar_t PATH_SEPARATOR = '.';

        LogSink* sink;

        std::wstring name;

    public:

        Logger(const std::string& name, Logger* parentLogger = nullptr)
            : sink(&LogSink::GetSink())
            , name(
                ((parentLogger == nullptr) ? std::wstring() : parentLogger->name)
                    + PATH_SEPARATOR
                    + std::wstring(name.begin(), name.end()))
        {
        }

        Logger(const std::wstring& name, Logger* parentLogger = nullptr)
            : sink(&LogSink::GetSink())
            , name(
                ((parentLogger == nullptr) ? std::wstring() : parentLogger->name)
                    + PATH_SEPARATOR
                    + name)
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

        template<typename... Params>
        void Critical(Params... params)
        {
            this->Write(LogLevel::Critical, params...);
        }

    private:

        template<typename Arg0, typename... Params>
        void Write(std::wostream& stream, LogLevel level, Arg0 arg0, Params... params)
        {
            this->WriteValue(stream, arg0);

            this->Write(stream, level, params...);
        }

        void Write(std::wostream& stream, LogLevel level)
        {
        }

        template<typename T>
        void WriteValue(
            std::wostream& stream,
            T value,
            typename std::enable_if<PreferWide<T>::value, T>::type* dummyPtr = nullptr)
        {
            stream << value;
        }

        template<typename T>
        void WriteValue(
            std::wostream& stream,
            T value,
            typename std::enable_if<PreferShort<T>::value, T>::type* dummyPtr = nullptr)
        {
            std::ostringstream substream;
            substream << value;
            auto str = substream.str();
            stream << std::wstring(str.begin(), str.end());
        }
/*
        template<typename T>
        void WriteValue(
            std::wostream& stream,
            T value,
            std::enable_if<
                !is_streamable<std::ostream, T>::value
                && !is_streamable<std::wostream, T>::value,
            T>* dummyPtr = nullptr)
        {
            if (sizeof(T) >= 0)
            {
                static_assert(false, "No stream operator defined!");
            }
        }
        */
    };
} }
