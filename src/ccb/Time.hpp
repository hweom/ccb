#pragma once

#include <chrono>
#include <iomanip>
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

        friend inline std::chrono::system_clock::duration operator - (const Time& t1, const Time& t2)
        {
            return t1.timePoint - t2.timePoint;
        }

        friend Time operator + (const Time& t1, const std::chrono::system_clock::duration& d)
        {
            return Time(t1.timePoint + d);
        }

        friend inline std::wostream& operator << (std::wostream& stream, const Time& t)
        {
            auto time = std::chrono::system_clock::to_time_t(t.timePoint);

            auto fields = std::localtime(&time);

            stream
                << std::setfill(L'0')
                << std::setw(4) << (1900 + fields->tm_year) << L"."
                << std::setw(2) << fields->tm_mon << L"."
                << std::setw(2) << fields->tm_mday << L" "
                << std::setw(2) << fields->tm_hour << L":"
                << std::setw(2) << fields->tm_min << L":"
                << std::setw(2) << fields->tm_sec;

            return stream;
        }

    public:

        static Time Now()
        {
            return Time(std::chrono::system_clock::now()) ;
        }

    };
}
