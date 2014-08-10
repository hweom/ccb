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

#include <chrono>
#include <iomanip>
#include <istream>
#include <ostream>
#include <stdexcept>

namespace ccb
{
    class Time
    {
    private:

        std::chrono::system_clock::time_point timePoint;

    public:

        Time()
        {
        }

        Time(const std::chrono::system_clock::time_point& timePoint)
            : timePoint(timePoint)
        {
        }

        Time(const struct timespec& timespec)
        {
            this->timePoint = std::chrono::system_clock::from_time_t(timespec.tv_sec) + std::chrono::nanoseconds(timespec.tv_nsec);
        }

        Time(int year, int month, int day, int hour = 0, int min = 0, int sec = 0, int msec = 0, int usec = 0, int nsec = 0)
        {
            std::tm fields;
            fields.tm_sec = sec;
            fields.tm_min = min;
            fields.tm_hour = hour;
            fields.tm_mday = day;
            fields.tm_mon = month - 1;
            fields.tm_year = year - 1900;
            fields.tm_isdst = -1;

            this->timePoint = std::chrono::system_clock::from_time_t(timegm(&fields))
                + std::chrono::milliseconds(msec)
                + std::chrono::microseconds(usec)
                + std::chrono::nanoseconds(nsec);
        }

    public:

        struct timespec ToTimespec() const
        {
            auto seconds = std::chrono::duration_cast<std::chrono::seconds>(
                this->timePoint.time_since_epoch());

            auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(
                this->timePoint.time_since_epoch() - seconds).count();

            struct timespec result;

            result.tv_sec = std::chrono::system_clock::to_time_t(std::chrono::system_clock::time_point(std::chrono::system_clock::duration(seconds)));
            result.tv_nsec = nanos;

            return result;
        }

        const std::chrono::system_clock::time_point& GetTimePoint() const
        {
            return this->timePoint;
        }

        Time& operator += (const std::chrono::system_clock::duration& d)
        {
            this->timePoint += d;
            return *this;
        }

        const Time Floor(const std::chrono::system_clock::duration& mod) const
        {
            std::chrono::system_clock::duration sysMod = mod;
            auto intervalCount = this->timePoint.time_since_epoch().count() / sysMod.count();
            return Time(std::chrono::system_clock::time_point() + sysMod * intervalCount);
        }

        friend inline bool operator == (const Time& t1, const Time& t2)
        {
            return t1.timePoint == t2.timePoint;
        }

        friend bool operator < (const Time& t1, const Time& t2)
        {
            return t1.timePoint < t2.timePoint;
        }

        friend bool operator <= (const Time& t1, const Time& t2)
        {
            return t1.timePoint <= t2.timePoint;
        }

        friend bool operator >= (const Time& t1, const Time& t2)
        {
            return t1.timePoint >= t2.timePoint;
        }

        friend bool operator > (const Time& t1, const Time& t2)
        {
            return t1.timePoint > t2.timePoint;
        }

        friend inline std::chrono::system_clock::duration operator - (const Time& t1, const Time& t2)
        {
            return t1.timePoint - t2.timePoint;
        }

        friend Time operator + (const Time& t1, const std::chrono::system_clock::duration& d)
        {
            return Time(t1.timePoint + d);
        }

        friend Time operator - (const Time& t1, const std::chrono::system_clock::duration& d)
        {
            return Time(t1.timePoint - d);
        }

        friend inline std::ostream& operator << (std::ostream& stream, const Time& t)
        {
            auto time = std::chrono::system_clock::to_time_t(t.timePoint);

            auto fields = std::localtime(&time);

            stream
                << std::setfill('0')
                << std::setw(4) << (1900 + fields->tm_year) << "."
                << std::setw(2) << (fields->tm_mon + 1) << "."
                << std::setw(2) << fields->tm_mday << "-"
                << std::setw(2) << fields->tm_hour << ":"
                << std::setw(2) << fields->tm_min << ":"
                << std::setw(2) << fields->tm_sec;

            return stream;
        }

        friend inline std::wostream& operator << (std::wostream& stream, const Time& t)
        {
            auto time = std::chrono::system_clock::to_time_t(t.timePoint);

            auto fields = std::localtime(&time);

            stream
                << std::setfill(L'0')
                << std::setw(4) << (1900 + fields->tm_year) << L"."
                << std::setw(2) << fields->tm_mon << L"."
                << std::setw(2) << fields->tm_mday << L"-"
                << std::setw(2) << fields->tm_hour << L":"
                << std::setw(2) << fields->tm_min << L":"
                << std::setw(2) << fields->tm_sec;

            return stream;
        }


        friend inline std::istream& operator >> (std::istream& stream, Time& t)
        {
            unsigned year, month, day, hour = 0, minute = 0, second = 0, millisecond = 0;

            stream >> year;
            if (stream.get() != '.')
            {
                throw std::invalid_argument("Not a time.");
            }

            stream >> month;
            if (stream.get() != '.')
            {
                throw std::invalid_argument("Not a time.");
            }

            stream >> day;
            if ((month < 1) || (month > 12) || (day < 1) || (day > 31))
            {
                throw std::invalid_argument("Not a time.");
            }

            if (stream.peek() == ' ')
            {
                stream.get();

                if (isdigit(stream.peek()))
                {
                    stream >> hour;
                    if (stream.get() != ':')
                    {
                        throw std::invalid_argument("Not a time.");
                    }

                    stream >> minute;
                    if (stream.get() != ':')
                    {
                        throw std::invalid_argument("Not a time.");
                    }

                    stream >> second;

                    if (stream.peek() == '.')
                    {
                        stream.get();
                        stream >> millisecond;
                    }
                }
            }

            t = Time(year, month, day, hour, minute, second, millisecond);

            return stream;
        }

    public:

        static Time Now()
        {
            return Time(std::chrono::system_clock::now()) ;
        }

    };
}

