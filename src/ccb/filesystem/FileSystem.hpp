#pragma once

#include <regex>

#include <ccb/filesystem/NotADirectoryException.hpp>
#include <ccb/filesystem/Path.hpp>

namespace ccb { namespace filesystem
{
    class FileSystem
    {
    public:

        bool FileExists(const Path& path) const;

        bool DirectoryExists(const Path &path) const;

        void CreateDirectories(const Path& path) const;

        void Rename(const Path& from, const Path& to) const;

        void Remove(const Path& path) const;

        void RemoveRecursive(const Path& path) const;

        Path GetTempPath() const;

        Path UniquePath(const std::wstring& model = L"%%%%-%%%%") const;

        std::vector<std::wstring> ReadDirectory(const Path &path, bool includeFiles = true, bool includeDirs = true);

        std::vector<std::wstring> ReadDirectoryFilter(const Path& path, const std::wregex& regex, bool includeFiles = true, bool includeDirs = true);
    };
} }
