#pragma once

#include <string>
#include <stdexcept>
#include <vector>

namespace ccb { namespace filesystem
{
    class Path
    {
    private:

        static const unsigned wchar_t nativeSeparator;

    private:

        std::vector<std::wstring> path;

        bool absolute = false;

    public:

        Path()
        {
        }

        Path(const std::string& path)
        {
            this->Decompose(std::wstring(path.begin(), path.end()));
        }

        Path(const std::wstring& path)
        {
            this->Decompose(path);
        }

    private:

        Path(
            std::vector<std::wstring>::const_iterator from,
            std::vector<std::wstring>::const_iterator to,
            bool absolute)
            : path(from, to)
            , absolute(absolute)
        {
        }

    public:

        bool IsEmpty() const
        {
            return path.size() == 0;
        }

        bool IsAbsolute() const
        {
            return this->absolute;
        }

        Path GetFilename() const
        {
            return this->path.back();
        }

        Path GetContainingPath() const
        {
            return Path(this->path.begin(), this->path.end()-1, this->absolute);
        }

        std::wstring ToString() const
        {
            std::wstring result;
            if (this->absolute)
            {
                result += nativeSeparator;
            }

            bool first = true;
            for (auto& step : this->path)
            {
                if (!first)
                {
                    result += nativeSeparator;
                }

                result += step;
                first = false;
            }

            return result;
        }

        std::string ToShortString() const
        {
            auto wideString = this->ToString();
            return std::string(wideString.begin(), wideString.end());
        }

        inline friend Path operator / (const Path& path1, const Path& path2)
        {
            if (path2.IsAbsolute())
            {
                throw std::logic_error("Cannot concatenate an absolute path");
            }

            Path result(path1);
            for (auto& step : path2.path)
            {
                result.path.push_back(step);
            }

            return result;
        }

    private:

        void Decompose(const std::wstring& path)
        {
            size_t start = 0;

            if (path.size() > 0)
            {
                if (path[0] == nativeSeparator)
                {
                    this->absolute = true;
                    start++;
                }

                auto pos = path.find_first_of(nativeSeparator, start);
                while (pos != std::wstring::npos)
                {
                    if (pos - start > 0)
                    {
                        this->path.push_back(path.substr(start, pos - start));
                    }

                    start = pos + 1;
                    pos = path.find_first_of(nativeSeparator, start);
                }

                if (start < path.length())
                {
                    this->path.push_back(path.substr(start, pos - start));
                }
            }
        }
    };
} }
