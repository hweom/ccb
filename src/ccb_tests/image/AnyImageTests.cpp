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

#include <ccb/image/AnyImage.hpp>
#include <ccb/image/ImageAlgorithm.hpp>

namespace ccb { namespace image
{
    class AnyImageTests : public CxxTest::TestSuite
    {
    public:

        void TestCheckType()
        {
            auto image = AnyImage::Create<Rgba8>(10, 10);

            TS_ASSERT(image.IsOfType<Rgba8>());
        }

        void TestViewAs()
        {
            auto image = AnyImage::Create<Rgba8>(100, 100);
            Image<Rgba8> image2(100, 100);

            auto image2View = image2.View();

            for (size_t i = 0; i < 100; i++)
            {
                auto r = image2View.BeginRow(i);
                for (size_t j = 0; j < 100; j++, r++)
                {
                    (*r)[0] = i;
                    (*r)[1] = 100 - i;
                    (*r)[2] = 255 - i;
                    (*r)[3] = 255;
                }
            }

            Copy(image.View<CompositePixel<uint8_t, Red>>(), image2.View<CompositePixel<uint8_t, Red>>());

            auto image1View = image.View<Rgba8>();
            for (size_t i = 0; i < 100; i++)
            {
                auto r = image1View.BeginRow(i);
                for (size_t j = 0; j < 100; j++, r++)
                {
                    TS_ASSERT_EQUALS(i, (*r)[0]);
                }
            }
        }

        void TestYuv8ToRgb8()
        {
            auto image = AnyImage::Create<Yuv8>(10, 10);

            auto p1 = image.View<Yuv8>().BeginRow(0);

            (*p1)[0] = 100;
            (*p1)[1] = 200;
            (*p1)[2] = 80;

            p1++;

            auto p2 = image.View<Rgb8>().BeginRow(0);

            TS_ASSERT_EQUALS(33, (*p2)[0]);
            TS_ASSERT_EQUALS(108, (*p2)[1]);
            TS_ASSERT_EQUALS(228, (*p2)[2]);
        }

        void TestYuv16ToRgb8()
        {
            auto image = AnyImage::Create<Yuv16>(10, 10);

            auto p1 = image.View<Yuv16>().BeginRow(0);

            (*p1)[0] = 25600;
            (*p1)[1] = 51200;
            (*p1)[2] = 20480;

            p1++;

            auto p2 = image.View<Rgb8>().BeginRow(0);

            TS_ASSERT_EQUALS(32, (*p2)[0]);
            TS_ASSERT_EQUALS(109, (*p2)[1]);
            TS_ASSERT_EQUALS(226, (*p2)[2]);
        }

        void TestRgb8ToYuv8()
        {
            auto image = AnyImage::Create<Rgb8>(10, 10);

            auto p1 = image.View<Rgb8>().BeginRow(0);

            (*p1)[0] = 33;
            (*p1)[1] = 108;
            (*p1)[2] = 228;

            p1++;

            auto p2 = image.View<Yuv8>().BeginRow(0);

            TS_ASSERT_EQUALS(99, (*p2)[0]);
            TS_ASSERT_EQUALS(200, (*p2)[1]);
            TS_ASSERT_EQUALS(80, (*p2)[2]);
        }

        void TestCmyk8ToRgb8()
        {
            auto image = AnyImage::Create<Cmyk8>(10, 10);

            auto p1 = image.View<Rgb8>().BeginRow(0);

            (*p1)[0] = 128;
            (*p1)[1] = 230;
            (*p1)[2] = 51;
            (*p1)[3] = 128;

            p1++;

            auto p2 = image.View<Rgb8>().BeginRow(0);

            TS_ASSERT_EQUALS(64, (*p2)[0]);
            TS_ASSERT_EQUALS(13, (*p2)[1]);
            TS_ASSERT_EQUALS(102, (*p2)[2]);
        }
    };
} }
