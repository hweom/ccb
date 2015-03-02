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

#include <ccb/stream/MemoryStream.hpp>

namespace ccb { namespace stream
{
    class MemoryStreamTests : public CxxTest::TestSuite
    {
    public:

        void TestCanRead()
        {
            char data[] = "hello";

            MemoryIStream<char> stream(data, 5);

            TS_ASSERT_EQUALS('h', stream.get());
            TS_ASSERT_EQUALS('e', stream.get());
            TS_ASSERT_EQUALS('l', stream.get());
            TS_ASSERT_EQUALS('l', stream.get());
            TS_ASSERT_EQUALS('o', stream.get());

            stream.get();

            TS_ASSERT(!stream);
        }

        void TestCanReadChunk()
        {
            char data[] = "hello";

            MemoryIStream<char> stream(data, 5);

            char buf[4];

            stream.read(buf, 4);

            for (size_t i = 0; i < sizeof(buf); i++)
            {
                TS_ASSERT_EQUALS(data[i], buf[i]);
            }
        }

        void TestCanTellg()
        {
            char data[] = "hello";

            MemoryIStream<char> stream(data, 5);

            stream.seekg(3);

            TS_ASSERT_EQUALS(3, static_cast<int>(stream.tellg()));
        }
    };
} }
