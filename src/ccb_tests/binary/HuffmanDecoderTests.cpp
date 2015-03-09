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
#include <ccb/binary/HuffmanDecoder.hpp>

namespace ccb { namespace binary
{
    class HuffmanDecoderTests : public CxxTest::TestSuite
    {
    public:

        void TestCanDecode()
        {
            auto decoder = HuffmanDecoder<int>
            {
                { "1", 1 },
                { "01", 2 },
                { "001", 3 },
                { "000", 4 }
            };

            auto bits = std::vector<uint8_t> { 0xa4, 0x40 };

            auto bitstream = MakeInputBitStream(bits.begin(), bits.end());

            TS_ASSERT_EQUALS(1, decoder.Next(bitstream));
            TS_ASSERT_EQUALS(2, decoder.Next(bitstream));
            TS_ASSERT_EQUALS(3, decoder.Next(bitstream));
            TS_ASSERT_EQUALS(4, decoder.Next(bitstream));
            TS_ASSERT_EQUALS(1, decoder.Next(bitstream));
            TS_ASSERT_EQUALS(4, decoder.Next(bitstream));
            TS_ASSERT_EQUALS(4, decoder.Next(bitstream));

            TS_ASSERT(!bitstream);
        }

    };
} }
