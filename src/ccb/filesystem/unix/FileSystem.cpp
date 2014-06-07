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

#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>

#include <ccb/filesystem/FileSystem.hpp>

namespace ccb { namespace filesystem
{
    template<typename Pred>
    std::vector<Path> QueryDirectory(const Path &path, const Pred& predicate)
    {
        std::vector<Path> result;

        auto pathStr = path.ToShortString();

        auto dp = opendir(pathStr.c_str());
        if (dp == nullptr)
        {
            throw NotADirectoryException();
        }

        struct dirent *entry;
        while ((entry = readdir(dp)))
        {
            std::string fileName(entry->d_name);

            if ((fileName == ".") || (fileName == ".."))
            {
                continue;
            }

            std::wstring wideFileName(fileName.begin(), fileName.end());

            struct stat st;
            if (stat((pathStr + "/" + fileName).c_str(), &st) == 0)
            {
                if (predicate(wideFileName, st))
                {
                    result.push_back(Path(wideFileName));
                }
            }
        }

        closedir(dp);

        return result;
    }

    bool RemoveDirectory(const std::string& path)
    {
        DIR *d = opendir(path.c_str());

        if (d == nullptr)
        {
            return false;
        }

        struct dirent *p;

        while ((p = readdir(d)))
        {
            struct stat statbuf;

            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
            {
                continue;
            }

            auto entryName = path + "/" + std::string(p->d_name);

            if (!stat(entryName.c_str(), &statbuf))
            {
                if (S_ISDIR(statbuf.st_mode))
                {
                    if (!RemoveDirectory(entryName))
                    {
                        return false;
                    }
                }
                else
                {
                    if (unlink(entryName.c_str()))
                    {
                        return false;
                    }
                }
            }
        }

        closedir(d);

        rmdir(path.c_str());

        return true;
    }

    bool FileSystem::FileExists(const Path &path) const
    {
        struct stat st;

        if (stat(path.ToShortString().c_str(), &st) == 0)
        {
            if (st.st_mode & S_IFREG != 0)
            {
                return true;
            }
        }

        return false;
    }

    bool FileSystem::DirectoryExists(const Path &path) const
    {
        struct stat st;

        if (stat(path.ToShortString().c_str(), &st) == 0)
        {
            if (st.st_mode & S_IFDIR != 0)
            {
                return true;
            }
        }

        return false;
    }

    void FileSystem::CreateDirectories(const Path& path) const
    {
        if (this->DirectoryExists(path))
        {
            return;
        }

        auto parentPath = path.GetContainingPath();
        this->CreateDirectories(parentPath);

        mkdir(path.ToShortString().c_str(), 0777);
    }

    void FileSystem::Rename(const Path& from, const Path& to) const
    {
        rename(from.ToShortString().c_str(), to.ToShortString().c_str());
    }

    void FileSystem::Remove(const Path& path) const
    {
        unlink(path.ToShortString().c_str());
    }

    void FileSystem::RemoveRecursive(const Path& path) const
    {
        RemoveDirectory(path.ToShortString());
    }

    Path FileSystem::GetTempPath() const
    {
        char const *folder = getenv("TMPDIR");
        if (folder == nullptr)
        {
            return Path(L"/tmp");
        }
        else
        {
            return Path(folder);
        }
    }

    Path FileSystem::UniquePath(const std::wstring& model) const
    {
        srand(time(NULL));

        std::wstring realPath;
        for (size_t i = 0; i < model.size(); i++)
        {
            if (model[i] == L'%')
            {
                auto symbol = rand() % (10 + ('z' - 'a') + 1);
                if (symbol < 10)
                {
                    realPath += '0' + symbol;
                }
                else
                {
                    realPath += 'a' + (symbol - 10);
                }
            }
            else
            {
                realPath += model[i];
            }
        }

        return realPath;
    }

    std::vector<Path> FileSystem::ReadDirectory(const Path &path, bool includeFiles, bool includeDirs)
    {
        return QueryDirectory(path, [includeFiles, includeDirs](const std::wstring& fileName, const struct stat& st)
        {
            return (includeDirs && S_ISDIR(st.st_mode)) ||
                (includeFiles && S_ISREG(st.st_mode));
        });
    }

    std::vector<Path> FileSystem::ReadDirectoryFilter(const Path& path, const std::wregex& regex, bool includeFiles, bool includeDirs)
    {
        return QueryDirectory(path, [includeFiles, includeDirs, &regex](const std::wstring& fileName, const struct stat& st)
        {
            auto match = std::regex_match(fileName, regex);
            if (!match)
            {
                return false;
            }

            return (includeDirs && S_ISDIR(st.st_mode))||
                (includeFiles && S_ISREG(st.st_mode));
        });
    }
} }
