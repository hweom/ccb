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

        void TestViewAs()
        {
            auto image = AnyImage::Create<Rgba8>(100, 100);

            auto rgbaView = image.ViewAs<Rgba8>();

            for (size_t i = 0; i < 100; i++)
            {
                auto r = rgbaView.BeginRow(i);
                for (size_t j = 0; j < 100; j++, r++)
                {
                    (*r).red = i;
                    (*r).green = 100 - i;
                    (*r).blue = 255 - i;
                    (*r).alpha = 255;
                }
            }
        }
    };
} }
