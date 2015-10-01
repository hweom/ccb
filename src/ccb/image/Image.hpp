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

#include <cstdlib>
#include <cstdint>
#include <vector>

#include <ccb/image/Bitmap.hpp>
#include <ccb/image/BitmapAlgorithm.hpp>
#include <ccb/image/BitmapView.hpp>
#include <ccb/image/ImageFormat.hpp>
#include <ccb/image/ImageFormatDescriptor.hpp>

namespace ccb { namespace image
{
    class Image
    {
    private:

        std::vector<uint8_t> data;

        ImageFormat format;

        size_t width;

        size_t height;

        size_t stride;


    public:

        Image(ImageFormat format, size_t width, size_t height)
            : format(format)
            , width(width)
            , height(height)
        {
            auto descriptor = ImageFormatDescriptor::Get(format);

            this->stride = (((width * descriptor.GetBitsPerPixel() + 7) / 8 + 3) / 4) * 4;

            this->data.resize(this->stride * this->height, 0);
        }

    public:

        template<typename PixelFormat>
        BitmapView<PixelFormat, PixelFormat> ViewAs()
        {
            return BitmapView<PixelFormat, PixelFormat>(this->data.data(), this->width, this->height, this->stride);
        }

        template<typename PixelFormat>
        Bitmap<PixelFormat> ToBitmap() const
        {
            Bitmap<PixelFormat> result(this->width, this->height);

            this->ApplyInverse<Copier, BitmapView<PixelFormat, PixelFormat>>(result.View());

            return result;
        }

    private:

        template<template<typename OpView1, typename OpView2> typename Op, typename View2>
        void ApplyInverse(View2&& view2) const
        {
            if (this->format == ImageFormat::Alpha1)
            {
                auto view = BitmapView<const Alpha1, typename View2::PixelType>(this->data.data(), this->width, this->height, this->stride);
                Op<View2, BitmapView<const Alpha1, typename View2::PixelType>>()(view2, view);
            }
        }
    };
} }
