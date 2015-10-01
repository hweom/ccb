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

#include <cassert>
#include <cstdlib>
#include <vector>

#include <ccb/image/ImageFormat.hpp>

namespace ccb { namespace image
{
    class ImageFormatDescriptor
    {
    private:

        ImageFormat format;

        size_t bitsPerPixel;

    private:

        ImageFormatDescriptor(ImageFormat format, size_t bitsPerPixel)
            : format(format)
            , bitsPerPixel(bitsPerPixel)
        {
        }

    public:

        size_t GetBitsPerPixel() const
        {
            return this->bitsPerPixel;
        }

    public:

        static ImageFormatDescriptor Get(ImageFormat format)
        {
            const auto& descriptors = GetDescriptors();

            auto index = static_cast<size_t>(format);

            assert (index > 0);
            assert (index <= descriptors.size());

            return descriptors[index-1];
        }

    private:

        static const std::vector<ImageFormatDescriptor>& GetDescriptors()
        {
            static std::vector<ImageFormatDescriptor> descriptors =
            {
                { ImageFormat::Gray1, 1 },
                { ImageFormat::Gray8, 8 },
                { ImageFormat::Gray16, 16 },
                { ImageFormat::Alpha1, 1 },
                { ImageFormat::Alpha8, 8 },
                { ImageFormat::Alpha16, 16 },
                { ImageFormat::Rgb8u, 24 },
                { ImageFormat::Rgb16u, 48 },
                { ImageFormat::Rgba8u, 32 },
                { ImageFormat::Rgba16u, 64 },
                { ImageFormat::Yuv8u, 24 },
                { ImageFormat::Yuv16u, 48 },
                { ImageFormat::Yuva8u, 32 },
                { ImageFormat::Yuva16u, 64 },
                { ImageFormat::Cmyk8u, 32 },
                { ImageFormat::Cmyk16u, 64 }
            };

            return descriptors;
        }

    };
} }
