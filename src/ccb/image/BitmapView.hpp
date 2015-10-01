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

#include <ccb/image/BitmapIterator.hpp>

namespace ccb { namespace image
{
    template<typename SrcPixel, typename DstPixel>
    class BitmapView
    {
    public:

        using SrcIter = BitmapIterator<SrcPixel>;

        using Iter = BitmapViewIterator<BitmapIterator<SrcPixel>, DstPixel>;

        using ByteType = typename std::conditional<std::is_const<SrcPixel>::value, const uint8_t*, uint8_t*>::type;

        using RawType = typename std::conditional<std::is_const<SrcPixel>::value, const void*, void*>::type;

        using ValueType = typename DstPixel::ValueType;

        using PixelType = DstPixel;

    private:

        ByteType data;

        size_t width;

        size_t height;

        size_t stride;

    public:

        BitmapView(RawType data, size_t width, size_t height, size_t stride)
            : data (reinterpret_cast<ByteType>(data))
            , width(width)
            , height(height)
            , stride(stride)
        {
        }

    public:

        size_t GetWidth() const
        {
            return this->width;
        }

        size_t GetHeight() const
        {
            return this->height;
        }

        Iter BeginRow(size_t row)
        {
            return Iter(SrcIter(this->data + this->stride * row));
        }

        Iter EndRow(size_t row)
        {
            return Iter(SrcIter(this->data + this->stride * row) + this->width);
        }
    };
} }
