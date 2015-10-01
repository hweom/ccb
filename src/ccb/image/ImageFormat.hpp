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

#pragma once

namespace ccb { namespace image
{
    enum class ImageFormat
    {
        Invalid = 0,

        Unknown = 0,

        Gray1 = 1,

        Gray8 = 2,

        Gray16 = 3,

        Alpha1 = 4,

        Alpha8 = 5,

        Alpha16 = 6,

        Rgb8u = 7,

        Rgb16u = 8,

        Rgba8u = 9,

        Rgba16u = 10,

        Yuv8u = 11,

        Yuv16u = 12,

        Yuva8u = 13,

        Yuva16u = 14,

        Cmyk8u = 15,

        Cmyk16u = 16
    };
} }
