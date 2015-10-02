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

#include <chrono>
#include <iostream>

#include <cxxtest/TestSuite.h>

#include <ccb/image/Image.hpp>

namespace ccb { namespace image
{
    class ImageTests : public CxxTest::TestSuite
    {
    public:

        void TestCanConvertAlpha1ToAlpha1()
        {
            auto image = Image<Alpha1>::Create<Alpha1>(10, 10);

            auto view = image.ViewAs<Alpha1>();
            for (size_t i = 0; i < 10; i++)
            {
                auto r = view.BeginRow(i);
                for (size_t j = 0; r != view.EndRow(i); r++, j++)
                {
                    (*r)[0] = ((i & 0x1) == 1) && ((j & 0x1) == 1);
                }
            }

            auto bitmap = image.ToBitmap<Alpha1>();

            auto view2 = bitmap.View();

            for (size_t i = 0; i < 10; i++)
            {
                auto r = view2.BeginRow(i);
                for (size_t j = 0; r != view2.EndRow(i); r++, j++)
                {
                    auto expected = ((i & 0x1) == 1) && ((j & 0x1) == 1);

                    TS_ASSERT_EQUALS(expected, (*r)[0]);
                }
            }
        }

        void TestConvertPerformance()
        {
            auto image1 = Image<Alpha1, Rgb8>::Create<Rgb8>(1000, 1000);

            uint64_t total = 0;

            for (size_t i = 0; i < 100; i++)
            {
                auto t1 = std::chrono::system_clock::now();

                auto image2 = image1.ToBitmap<Rgb8>();

                auto t2 = std::chrono::system_clock::now();

                total += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
            }

            std::cout << std::endl << "Image copy average on " << static_cast<double>(total) / 100 << " micros" << std::endl;
        }
    };
} }
