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

#include <regex>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <ccb/filesystem/NotADirectoryException.hpp>
#include <ccb/filesystem/Path.hpp>

namespace ccb { namespace filesystem
{
    class FileSystem
    {
    public:

        bool FileExists(const Path& path) const
        {
#ifdef _WIN32
            WIN32_FILE_ATTRIBUTE_DATA data;

            if (GetFileAttributesEx(path.ToString().c_str(), GetFileExInfoStandard, &data) != 0)
            {
                return (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
            }
#else
            struct stat st;

            if (stat(path.ToShortString().c_str(), &st) == 0)
            {
                if ((st.st_mode & S_IFREG) != 0)
                {
                    return true;
                }
            }
#endif

            return false;
        }

        bool DirectoryExists(const Path &path) const
        {
#ifdef _WIN32
            WIN32_FILE_ATTRIBUTE_DATA data;

            if (GetFileAttributesEx(path.ToString().c_str(), GetFileExInfoStandard, &data) != 0)
            {
                return (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            }
#else
            struct stat st;

            if (stat(path.ToShortString().c_str(), &st) == 0)
            {
                if (st.st_mode & S_IFDIR != 0)
                {
                    return true;
                }
            }
#endif

            return false;
        }

        void CreateDirectories(const Path& path) const
        {
            if (path.IsEmpty() || this->DirectoryExists(path))
            {
                return;
            }

            auto parentPath = path.GetContainingPath();
            this->CreateDirectories(parentPath);

#ifdef _WIN32
            if (CreateDirectory(path.ToString().c_str(), nullptr) == 0)
            {
                throw std::system_error(
                    GetLastError(),
                    std::system_category(),
                    "error while trying to create path " + path.ToShortString());
            }   
#else
            if (mkdir(path.ToShortString().c_str(), 0777) != 0)
            {
                throw std::system_error(
                    errno,
                    std::system_category(),
                    "error while trying to create path " + path.ToShortString());
            }
#endif
        }

        void Remove(const Path& path) const
        {
#ifdef _WIN32
            WIN32_FILE_ATTRIBUTE_DATA data;

            if (GetFileAttributesEx(path.ToString().c_str(), GetFileExInfoStandard, &data) != 0)
            {
                if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    RemoveDirectory(path.ToString().c_str());
                }
                else
                {
                    DeleteFile(path.ToString().c_str());
                }
            }
#else
            unlink(path.ToShortString().c_str());
#endif
        }

        std::vector<Path> ReadDirectoryFilter(const Path& path, const std::wregex& regex, bool includeFiles = true, bool includeDirs = true)
        {
#ifdef _WIN32
            return this->QueryDirectory(path, [includeFiles, includeDirs, &regex](const WIN32_FIND_DATA& data)
            {
                auto match = std::regex_match(data.cFileName, regex);
                if (!match)
                {
                    return false;
                }

                return 
                    (includeDirs && ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) ||
                    (includeFiles && ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)));
            });
#else
            return this->QueryDirectory(path, [includeFiles, includeDirs, &regex](const std::wstring& fileName, const struct stat& st)
            {
                auto match = std::regex_match(fileName, regex);
                if (!match)
                {
                    return false;
                }

                return (includeDirs && S_ISDIR(st.st_mode)) ||
                    (includeFiles && S_ISREG(st.st_mode));
            });
#endif
        }

#ifndef _WIN32

        void Rename(const Path& from, const Path& to) const
        {
            rename(from.ToShortString().c_str(), to.ToShortString().c_str());
        }

        void RemoveRecursive(const Path& path) const
        {
            RemoveDirectory(path.ToShortString());
        }

        Path GetTempPath() const
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

        Path UniquePath(const std::wstring& model = L"%%%%-%%%%") const
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

        std::vector<Path> ReadDirectory(const Path &path, bool includeFiles = true, bool includeDirs = true)
        {
            return QueryDirectory(path, [includeFiles, includeDirs](const std::wstring& fileName, const struct stat& st)
            {
                return (includeDirs && S_ISDIR(st.st_mode)) ||
                    (includeFiles && S_ISREG(st.st_mode));
            });
        }
#endif

    private:

#ifdef _WIN32
        template<typename Pred>
        std::vector<Path> QueryDirectory(const Path &path, const Pred& predicate)
        {
            std::vector<Path> result;

            auto pathStr = path.ToString() + L"\\*";

            WIN32_FIND_DATA data;

            auto handle = FindFirstFileEx(
                pathStr.c_str(),
                FindExInfoBasic,
                &data,
                FindExSearchNameMatch,
                nullptr,
                0);

            if (handle == INVALID_HANDLE_VALUE)
            {
                return std::vector<Path>();
            }

            if (predicate(data))
            {
                result.push_back(Path(std::wstring(data.cFileName)));
            }

            while (FindNextFile(handle, &data))
            {
                if (predicate(data))
                {
                    result.push_back(Path(std::wstring(data.cFileName)));
                }
            }

            FindClose(handle);

            return result;
        }
#else
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
#endif


    };
} }
