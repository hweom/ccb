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
