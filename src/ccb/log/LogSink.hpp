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

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <list>
#include <mutex>
#include <set>
#include <thread>

#include <ccb/Time.hpp>
#include <ccb/log/ILogTarget.hpp>
#include <ccb/log/LogLevel.hpp>

namespace ccb { namespace log
{
    class LogSink
    {
    private:

        struct LogEntry
        {
            Time time;

            LogLevel level;

            std::wstring source;

            std::wstring message;

            LogEntry(LogLevel level, const std::wstring& source, const std::wstring& message)
                : time(std::chrono::system_clock::now())
                , level(level)
                , source(source)
                , message(message)
            {
            }
        };

    private:

        std::mutex dispatchMutex;

        std::mutex targetMutex;

        std::condition_variable entriesUpdated;

        std::list<LogEntry> entries;

        std::atomic<bool> exitDispatcher;

        std::thread dispatchThread;

        std::set<ILogTarget*> targets;

    public:

        LogSink()
            : exitDispatcher(false)
            , dispatchThread(&LogSink::DispatchThread, this)
        {
        }

        ~LogSink()
        {
            this->exitDispatcher.store(true);
            this->entriesUpdated.notify_all();

            this->dispatchThread.join();
        }

    public:

        void AddTarget(ILogTarget* target)
        {
            std::lock_guard<std::mutex> lock(this->targetMutex);

            this->targets.insert(target);
        }

        void RemoveTarget(ILogTarget* target)
        {
            std::lock_guard<std::mutex> lock(this->targetMutex);

            this->targets.erase(target);
        }

        void WriteMessage(LogLevel level, const std::wstring& source, const std::wstring& message)
        {
            std::lock_guard<std::mutex> lock(this->dispatchMutex);

            this->entries.emplace_back(level, source, message);
            this->entriesUpdated.notify_all();
        }

    public:

        static LogSink& GetSink()
        {
            static LogSink sink;
            return sink;
        }

    private:

        void DispatchThread()
        {
            while (!this->exitDispatcher.load())
            {
                auto entries = this->GetAllEntries();
                for (auto& entry : entries)
                {
                    this->DeliverEntry(entry);
                }
            }
        }

        void DeliverEntry(const LogEntry& entry)
        {
            std::lock_guard<std::mutex> lock(this->targetMutex);

            for (auto target : this->targets)
            {
                target->LogMessage(entry.time, entry.level, entry.source, entry.message);
            }
        }

        std::list<LogEntry> GetAllEntries()
        {
            std::list<LogEntry> result;

            std::unique_lock<std::mutex> lock(this->dispatchMutex);

            if (this->entries.size() == 0)
            {
                this->entriesUpdated.wait(
                    lock,
                    [this]()
                    {
                        return (this->entries.size() > 0) || this->exitDispatcher.load();
                    });
            }

            this->entries.swap(result);

            return result;
        }
    };
} }
