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

#include <ccb/image/Image.hpp>
#include <ccb/image/ImageAlgorithm.hpp>

namespace ccb { namespace image
{
    class ImageAlgorithmTests : public CxxTest::TestSuite
    {
    public:

        void TestBitIterator()
        {
            Image<Alpha1> image(100, 100);

            auto view = image.View();

            // Fill image.
            for (size_t i = 0; i < 100; i++)
            {
                auto r = view.BeginRow(i);
                for (size_t j = 0; r != view.EndRow(i); r++, j++)
                {
                    (*r)[0] = ((i & 0x1) == 1) && ((j & 0x1) == 1);
                }
            }

            auto view2 = static_cast<const Image<Alpha1>&>(image).View();

            // Check image.
            for (size_t i = 0; i < 100; i++)
            {
                auto r = view2.BeginRow(i);
                for (size_t j = 0; r != view2.EndRow(i); r++, j++)
                {
                    auto expected = ((i & 0x1) == 1) && ((j & 0x1) == 1);

                    TS_ASSERT_EQUALS(expected, (*r)[0]);
                }
            }
        }

        void TestCopyAlphaFromBitAlphaToRgba()
        {
            Image<Rgba8> dest(100, 100);
            Image<Alpha1> src(100, 100);

            auto sview = src.View();

            // Fill source image.
            for (size_t i = 0; i < 100; i++)
            {
                auto r = sview.BeginRow(i);
                for (size_t j = 0; r != sview.EndRow(i); r++, j++)
                {
                    (*r)[0] = ((i & 0x1) == 1) && ((j & 0x1) == 1);
                }
            }

            auto dview = dest.View<Alpha1>();

            Copy(dview, sview);

            auto dview2 = dest.View();

            // Check image.
            for (size_t i = 0; i < 100; i++)
            {
                auto r = dview2.BeginRow(i);
                for (size_t j = 0; r != dview2.EndRow(i); r++, j++)
                {
                    auto expected = ((i & 0x1) == 1) && ((j & 0x1) == 1) ? 0xff : 0x00;

                    if (expected != (*r)[3])
                    {
                        TS_ASSERT_EQUALS(expected, (*r)[3]);
                    }
                }
            }
        }
    };
} }
