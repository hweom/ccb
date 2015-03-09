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

#include <ccb/binary/InputBitStream.hpp>

namespace ccb { namespace binary
{
    class InputBitStreamTests : public CxxTest::TestSuite
    {
    public:

        void TestCanInputBits()
        {
            std::vector<uint8_t> bytes = { 0xAA, 0x55 };

            std::vector<bool> expected =
            {
                true,
                false,
                true,
                false,
                true,
                false,
                true,
                false,
                false,
                true,
                false,
                true,
                false,
                true,
                false,
                true
            };

            auto bitstream = MakeInputBitStream(bytes.begin(), bytes.end());

            for (auto bit : expected)
            {
                TS_ASSERT(bitstream.IsGood());

                TS_ASSERT_EQUALS(bit, bitstream.Read());
            }

            TS_ASSERT(!bitstream.IsGood());
        }

        void TestCanInputFromSigned()
        {
            std::string bytes = "U";

            std::vector<bool> expected =
            {
                false,
                true,
                false,
                true,
                false,
                true,
                false,
                true
            };

            auto bitstream = MakeInputBitStream(bytes.begin(), bytes.end());

            for (auto bit : expected)
            {
                TS_ASSERT(bitstream.IsGood());

                TS_ASSERT_EQUALS(bit, bitstream.Read());
            }

            TS_ASSERT(!bitstream.IsGood());
        }

    };
} }
