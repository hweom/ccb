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

#include <cxxtest/TestSuite.h>

#include <ccb/csv/CsvFile.hpp>
#include <ccb/filesystem/FileSystem.hpp>

namespace ccb { namespace csv
{
    class CsvFileTests : public CxxTest::TestSuite
    {
    public:

        void TestCanReadAfterWrite()
        {
            filesystem::FileSystem filesystem;
            auto tempFile = filesystem.GetTempPath() / filesystem.UniquePath();

            CsvFile file;
            file.Add() << 1 << 23.4;
            file.Add() << "Hello" << 'a';

            file.Save(tempFile);

            CsvFile file2(tempFile);
            int idx = 0;
            for (auto row : file2)
            {
                if (idx == 0)
                {
                    int num;
                    double d;
                    row >> num >> d;

                    TS_ASSERT_EQUALS(1, num);
                    TS_ASSERT_EQUALS(23.4, d);
                }
                else if (idx == 1)
                {
                    std::string str;
                    char c;
                    row >> str >> c;

                    TS_ASSERT_EQUALS("Hello", str);
                    TS_ASSERT_EQUALS('a', c);
                }
                else
                {
                    TS_FAIL("");
                }

                idx++;
            }

            filesystem.Remove(tempFile);
        }
    };
} }
