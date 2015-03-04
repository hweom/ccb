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

#include <ccb/stream/ContainerStream.hpp>

namespace ccb { namespace stream
{
    class ContainerStreamTests : public CxxTest::TestSuite
    {
    public:

        void TestCanWriteToEmptyVector()
        {
            std::vector<char> data;

            ContainerOStream<char, std::vector<char>> stream(data);

            stream << "hello";

            TS_ASSERT_EQUALS(5, data.size());
        }

        void TestCanWriteWithPut()
        {
            std::vector<char> data;

            ContainerOStream<char, std::vector<char>> stream(data);

            for (size_t i = 0; i < 10; i++)
            {
                stream.put(i);
            }

            TS_ASSERT_EQUALS(10, data.size());

            for (size_t i = 0; i < 10; i++)
            {
                TS_ASSERT_EQUALS(i, data[i]);
            }
        }

        void TestCanOverwriteData()
        {
            std::vector<char> data = { 'h', 'e', 'l', 'l', 'o' };

            ContainerOStream<char, std::vector<char>> stream(data);

            stream << "m";

            TS_ASSERT_EQUALS("mello", std::string(data.data(), 5));
        }

        void TestCanWriteSeek()
        {
            std::vector<char> data;

            ContainerOStream<char, std::vector<char>> stream(data);

            stream << "hello";

            stream.seekp(0);

            stream << "m";

            stream.seekp(0, std::ios_base::end);

            stream << "n";

            TS_ASSERT_EQUALS("mellon", std::string(data.data(), 6));
        }

        void TestCanWriteToVectorOfDifferentType()
        {
            std::vector<uint8_t> data;

            ContainerOStream<char, std::vector<uint8_t>> stream(data);

            stream << "hello";

            TS_ASSERT_EQUALS(5, data.size());
        }
    };
} }
