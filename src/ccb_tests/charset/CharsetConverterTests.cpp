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

#include <ccb/charset/CharsetConverter.hpp>

namespace ccb { namespace charset
{
    class CharsetConverterTests : public CxxTest::TestSuite
    {
    public:

        void TestCanDecodeUtf8WithBom()
        {
            auto input = std::vector<uint8_t>({ 0xef, 0xbb, 0xbf, 'a', 'b', 'c', 'd', 0xd0, 0xb0 });

            auto converted = std::wstring();

            CharsetConverter<Encoding::UTF32, Encoding::Unknown>().ConvertBytes(input.begin(), input.end(), std::back_inserter(converted));

            TS_ASSERT_EQUALS(L"abcd\u0430", converted);
        }

        void TestCanDecodeUtf16WithBom()
        {
            auto input = std::vector<uint8_t>({ 0xff, 0xfe, 'a', 0, 'b', 0, 'c', 0, 'd', 0 });

            auto converted = std::wstring();

            CharsetConverter<Encoding::UTF32, Encoding::Unknown>().ConvertBytes(input.begin(), input.end(), std::back_inserter(converted));

            TS_ASSERT_EQUALS(L"abcd", converted);
        }
    };
} }
