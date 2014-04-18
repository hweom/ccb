#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <list>
#include <mutex>
#include <set>
#include <thread>

#include <cloudcastle/infrastructure/Time.hpp>
#include <cloudcastle/infrastructure/log/ILogTarget.hpp>
#include <cloudcastle/infrastructure/log/LogLevel.hpp>

namespace cloudcastle { namespace infrastructure { namespace log
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
} } }
