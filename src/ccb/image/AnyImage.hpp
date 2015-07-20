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

#include <cstdint>
#include <cstdlib>
#include <vector>

#include <ccb/image/Image.hpp>
#include <ccb/meta/TypeList.hpp>

#define IMAGE_REGISTER_LAYOUT(Layout) Layout<bool>, Layout<uint8_t>

namespace ccb { namespace image
{
    namespace placeholder
    {
        struct Im1 {};
    }

    namespace details
    {
        using PixelDataTypes = meta::TypeList<
            RgbaPixel<uint8_t>,
            MonochromePixel<Alpha, bool>>;
    }

    class AnyImage
    {
    private:

        std::vector<uint8_t> data;

        size_t width;

        size_t height;

        size_t stride;

        unsigned typeCode;

    private:

        AnyImage(std::vector<uint8_t>&& data, size_t width, size_t height, size_t stride, unsigned typeCode)
            : data(std::move(data))
            , width(width)
            , height(height)
            , stride(stride)
            , typeCode(typeCode)
        {
        }

    public:

        template<typename Pixel>
        Image<Pixel> ReinterpretAs() &&
        {
            return Image<Pixel>(std::move(this->data), width, height, stride);
        }

        /// Get image view.
        template<typename SrcPixel, typename ViewPixel = SrcPixel>
        ImageView<SrcPixel, ViewPixel> ViewAs()
        {
            return ImageView<SrcPixel, ViewPixel>(this->data.data(), this->width, this->height, this->stride);
        }

        /// Get const image view.
        template<typename SrcPixel, typename ViewPixel = SrcPixel>
        ImageView<const SrcPixel, ViewPixel> ViewAs() const
        {
            return ImageView<const SrcPixel, ViewPixel>(this->data.data(), this->width, this->height, this->stride);
        }

    public:

        template<typename Pixel>
        static AnyImage Create(size_t width, size_t height)
        {
            auto bpp = PixelTraits<Pixel>::BitsPerPixel;
            auto stride = (((width * bpp + 7) / 8 + 3) / 4) * 4;
            auto typeCode = meta::Find<Pixel, details::PixelDataTypes>::value;
            if (typeCode == meta::Npos::value)
            {
                throw std::logic_error("Unregistered pixel type");
            }

            return AnyImage(std::vector<uint8_t>(stride * height), width, height, stride, typeCode);
        }

        template<typename Pixel>
        static AnyImage Create(std::vector<uint8_t>&& data, size_t width, size_t height, size_t stride)
        {
            auto typeCode = meta::Find<details::PixelDataTypes, Pixel>::value;
            if (typeCode == meta::Npos::value)
            {
                throw std::logic_error("Unregistered pixel type");
            }

            return AnyImage(std::move(data), width, height, stride, typeCode);
        }

        template<typename ViewPixel, typename Func, typename... Params>
        friend void ApplyAs(Func func, AnyImage& image, Params&&... params);
    };

    namespace details
    {
        template<typename Pixel, typename ViewPixel, typename T>
        struct FillPlaceholder1
        {
            constexpr T&& operator ()(AnyImage& image, typename std::remove_reference<T>::type& t)
            {
                return static_cast<T&&>(t);
            }
        };

        template<typename Pixel, typename ViewPixel>
        struct FillPlaceholder1<Pixel, ViewPixel, placeholder::Im1>
        {
            ImageView<Pixel, ViewPixel> operator ()(AnyImage& image, placeholder::Im1 t)
            {
                return image.ViewAs<Pixel, ViewPixel>();
            }
        };

        template<typename Pixel, typename ViewPixel, typename Func, typename... Params>
        void ResolveAndApply(Func func, AnyImage& image, Params&&... params)
        {
            auto pos = meta::Find<PixelDataTypes, Pixel>::value;
            if (image.typeCode == pos)
            {
                func(details::FillPlaceholder1<Pixel, ViewPixel, Params>()(image, std::forward<Params>(params))...);
            }
            else if (pos == meta::Npos::value)
            {
                throw std::logic_error("Unregistered pixel type");
            }
            else
            {
                ResolveAndApply<typename meta::Next<PixelDataTypes, Pixel>::type, ViewPixel>(func, image, std::forward<Params>(params)...);
            }
        }
    }

    template<typename ViewPixel, typename Func, typename... Params>
    inline void ApplyAs(Func func, AnyImage& image, Params&&... params)
    {
        details::ResolveAndApply<typename meta::Front<details::PixelDataTypes>::type, ViewPixel>(func, image, std::forward<Params>(params)...);
    }
} }
