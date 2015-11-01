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
    namespace details
    {
        struct ImageInfo
        {
            size_t componentCount;

            size_t bitsPerPixel;

            bool hasAlpha;
        };

        template<typename... PixelTypes>
        struct ImageInfoProvider
        {
            ImageInfo operator () (unsigned typeCode)
            {
                throw std::logic_error("Image type not registered");
            }
        };

        template<typename T, typename... PixelTypes>
        struct ImageInfoProvider<T, PixelTypes...>
        {
            ImageInfo operator () (unsigned typeCode)
            {
                if (typeCode == 0)
                {
                    return ImageInfo
                    {
                        PixelTraits<T>::ComponentCount,
                        PixelTraits<T>::BitsPerPixel,
                        PixelChannelTraits<T, Alpha>::HasChannel
                    };
                }
                else
                {
                    return ImageInfoProvider<PixelTypes...>()(typeCode - 1);
                }
            }
        };

        template<template<typename OpView1, typename OpView2> class Op, typename View, typename... PixelTypes>
        struct ConstApplier
        {
            void operator ()(
                View&& view,
                unsigned typeCode,
                const void* data,
                size_t width,
                size_t height,
                size_t stride)
            {
                throw std::logic_error("Image type not registered");
            }
        };

        template<template<typename OpView1, typename OpView2> class Op, typename View, typename First, typename... PixelTypes>
        struct ConstApplier<Op, View, First, PixelTypes...>
        {
            void operator ()(
                View&& view,
                unsigned typeCode,
                const void* data,
                size_t width,
                size_t height,
                size_t stride)
            {
                if (typeCode == 0)
                {
                    auto view2 = BitmapView<const First, typename View::PixelType>(data, width, height, stride);
                    Op<BitmapView<const First, typename View::PixelType>, View>()(view2, view);
                }
                else
                {
                    return ConstApplier<Op, View, PixelTypes...>()(std::forward<View>(view), typeCode - 1, data, width, height, stride);
                }
            }
        };

        template<template<typename OpView1, typename OpView2> class Op, typename View2, typename... PixelTypes>
        struct ConstReverseApplier
        {
            void operator ()(
                unsigned typeCode,
                const void* data,
                size_t width,
                size_t height,
                size_t stride,
                View2&& view2)
            {
                throw std::logic_error("Image type not registered");
            }
        };

        template<template<typename OpView1, typename OpView2> class Op, typename View2, typename First, typename... PixelTypes>
        struct ConstReverseApplier<Op, View2, First, PixelTypes...>
        {
            void operator ()(
                unsigned typeCode,
                const void* data,
                size_t width,
                size_t height,
                size_t stride,
                View2&& view2)
            {
                if (typeCode == 0)
                {
                    auto view = BitmapView<const First, typename View2::PixelType>(data, width, height, stride);
                    Op<View2, BitmapView<const First, typename View2::PixelType>>()(view2, view);
                }
                else
                {
                    return ConstReverseApplier<Op, View2, PixelTypes...>()(typeCode - 1, data, width, height, stride, std::forward<View2>(view2));
                }
            }
        };
    }

    template<typename... PixelTypes>
    class Image
    {
    private:

        std::vector<uint8_t> data;

        unsigned typeCode = 0;

        size_t width;

        size_t height;

        size_t stride;


    public:

        Image()
        {
        }

        Image(Image&& other)
            : data(std::move(other.data))
            , width(other.width)
            , height(other.height)
            , stride(other.stride)
            , typeCode(other.typeCode)
        {
        }

    private:

        Image(std::vector<uint8_t>&& data, size_t width, size_t height, size_t stride, unsigned typeCode)
            : data(std::move(data))
            , width(width)
            , height(height)
            , stride(stride)
            , typeCode(typeCode)
        {
        }

    public:

        bool IsEmpty() const
        {
            return this->data.empty();
        }

        template<typename PixelType>
        bool IsOfType() const
        {
            return details::Offset<PixelType, PixelTypes...>::value == this->typeCode;
        }

        size_t GetSize() const
        {
            return this->data.size();
        }

        size_t GetWidth() const
        {
            return this->width;
        }

        size_t GetHeight() const
        {
            return this->height;
        }

        size_t GetStride() const
        {
            return this->stride;
        }

        size_t GetComponentCount() const
        {
            return details::ImageInfoProvider<PixelTypes...>()(this->typeCode).componentCount;
        }

        size_t GetBitsPerPixel() const
        {
            return details::ImageInfoProvider<PixelTypes...>()(this->typeCode).bitsPerPixel;
        }

        uint8_t* GetData()
        {
            return this->data.data();
        }

        const uint8_t* GetData() const
        {
            return this->data.data();
        }

        template<typename SrcFormat, typename DstFormat = SrcFormat>
        BitmapView<SrcFormat, DstFormat> ViewAs()
        {
            return BitmapView<SrcFormat, DstFormat>(this->data.data(), this->width, this->height, this->stride);
        }

        template<typename SrcFormat, typename DstFormat = SrcFormat>
        BitmapView<const SrcFormat, const DstFormat> ViewAs() const
        {
            return BitmapView<const SrcFormat, const DstFormat>(this->data.data(), this->width, this->height, this->stride);
        }

        template<typename PixelFormat>
        Bitmap<PixelFormat> ToBitmap() const
        {
            Bitmap<PixelFormat> result(this->width, this->height);

            details::ConstReverseApplier<Copier, BitmapView<PixelFormat, PixelFormat>, PixelTypes...>()(
                this->typeCode,
                this->data.data(),
                this->width,
                this->height,
                this->stride,
                result.View());

            return result;
        }

        template<typename SrcPixel, typename DstPixel>
        void CopyFrom(BitmapView<const SrcPixel, const DstPixel>&& view)
        {
            details::ConstApplier<Copier, BitmapView<const SrcPixel, const DstPixel>, PixelTypes...>()(
                std::move(view),
                this->typeCode,
                this->data.data(),
                this->width,
                this->height,
                this->stride);
        }

    public:

        template<typename Pixel>
        static Image Create(size_t width, size_t height)
        {
            auto bpp = PixelTraits<Pixel>::BitsPerPixel;
            auto stride = (((width * bpp + 7) / 8 + 3) / 4) * 4;
            auto typeCode = details::Offset<Pixel, PixelTypes...>::value;
            if (typeCode == details::Npos::value)
            {
                throw std::logic_error("Unregistered pixel type");
            }

            return Image(std::vector<uint8_t>(stride * height), width, height, stride, typeCode);
        }

        template<typename Pixel>
        static Image Create(std::vector<uint8_t>&& data, size_t width, size_t height, size_t stride)
        {
            auto typeCode = details::Offset<Pixel, PixelTypes...>::value;
            if (typeCode == details::Npos::value)
            {
                throw std::logic_error("Unregistered pixel type");
            }

            return Image(std::move(data), width, height, stride, static_cast<unsigned>(typeCode));
        }
    };
} }
