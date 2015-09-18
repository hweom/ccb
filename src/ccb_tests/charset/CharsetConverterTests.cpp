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

        void TestCanDecodeEncodeCp866()
        {
            auto map = std::unordered_map<uint8_t, uint32_t>(
            {
                { 0x13, 0x13 },
                { 0x28, 0x28 },
                { 0x7f, 0x7f },
                { 0x80, 0x0410 },
                { 0x99, 0x0429 },
                { 0xB2, 0x2593 },
                { 0xD6, 0x2553 },
                { 0xF3, 0x0454 },
            });

            this->CheckEncodeDecode<Encoding::CP_866>(map);
        }

        void TestCanDecodeEncodeCp1251()
        {
            auto map = std::unordered_map<uint8_t, uint32_t>(
            {
                { 0x13, 0x13 },
                { 0x28, 0x28 },
                { 0x7f, 0x7f },
                { 0x80, 0x0402 },
                { 0x99, 0x2122 },
                { 0xB2, 0x0406 },
                { 0xD6, 0x0426 },
                { 0xF3, 0x0443 },
            });

            this->CheckEncodeDecode<Encoding::CP_1251>(map);
        }

        void TestCanDecodeEncodeKoi8()
        {
            auto map = std::unordered_map<uint8_t, uint32_t>(
            {
                { 0x13, 0x13 },
                { 0x28, 0x28 },
                { 0x7f, 0x7f },
                { 0x80, 0x2500 },
                { 0x99, 0x2265 },
                { 0xB2, 0x2561 },
                { 0xD6, 0x0436 },
                { 0xF3, 0x0421 },
            });

            this->CheckEncodeDecode<Encoding::KOI8_R>(map);
        }

    protected:

        template<Encoding Enc>
        void CheckEncodeDecode(const std::unordered_map<uint8_t, uint32_t>& cases)
        {
            CharsetConverter<Encoding::UTF32, Enc> converter;

            for (const auto& pair : cases)
            {
                std::string s1;
                std::wstring r1;

                s1.push_back(pair.first);
                converter.ConvertBytes(s1.begin(), s1.end(), std::back_inserter(r1));

                TS_ASSERT_EQUALS(1, r1.size());
                TS_ASSERT_EQUALS(pair.second, r1[0]);
            }
        }
    };
} }
