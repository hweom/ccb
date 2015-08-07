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

#include <ccb/image/ImageView.hpp>

namespace ccb { namespace image
{
    template<typename Pixel>
    class Image
    {
    private:

        std::vector<uint8_t> data;

        size_t width;

        size_t height;

        size_t stride;

    public:

        Image()
        {
        }

        Image(size_t width, size_t height)
            : width(width)
            , height(height)
        {
            auto bpp = this->GetBitsPerPixel();

            this->stride = (((width * bpp + 7) / 8 + 3) / 4) * 4;

            this->data.resize(this->stride * this->height, 0);
        }

        Image(std::vector<uint8_t>&& data, size_t width, size_t height, size_t stride)
            : data(std::move(data))
            , width(width)
            , height(height)
            , stride(stride)
        {
        }

        Image(Image&& other) _NOEXCEPT
            : data(std::move(other.data))
            , width(other.width)
            , height(other.height)
            , stride(other.stride)
        {
        }

        template<typename View>
        Image(View view)
            : width(view.GetWidth())
            , height(view.GetHeight())
        {
            auto bpp = this->GetBitsPerPixel();

            this->stride = (((width * bpp + 7) / 8 + 3) / 4) * 4;

            this->data.resize(this->stride * this->height, 0);

            auto thisView = this->View();

            for (size_t i = 0; i < this->height; i++)
            {
                auto r1 = thisView.BeginRow(i);
                auto r2 = view.BeginRow(i);

                for (size_t j = 0; j < this->width; j++, r1++, r2++)
                {
                    PixelConverter<typename std::remove_const<typename View::PixelType>::type, Pixel>()((*r1), (*r2));
                }
            }
        }

        /// Disable copy constructor to disallow expensive image copying.
        Image(const Image& other) = delete;

        /// Disable copy constructor to disallow expensive image copying.
        Image& operator = (const Image& other) = delete;

    public:

        bool IsEmpty() const
        {
            return this->data.empty();
        }

        size_t GetSize() const
        {
            return this->data.size();
        }

        uint32_t GetWidth() const
        {
            return this->width;
        }

        uint32_t GetHeight() const
        {
            return this->height;
        }

        uint32_t GetStride() const
        {
            return this->stride;
        }

        /// Get bits per pixel number.
        size_t GetBitsPerPixel() const
        {
            return PixelTraits<Pixel>::BitsPerPixel;
        }

        const uint8_t* GetData() const
        {
            return this->data.data();
        }

        uint8_t* GetData()
        {
            return this->data.data();
        }

        /// Get image view.
        template<typename ViewPixel = Pixel>
        ImageView<Pixel, ViewPixel> View()
        {
            return ImageView<Pixel, ViewPixel>(this->data.data(), this->width, this->height, this->stride);
        }

        /// Get const image view.
        template<typename ViewPixel = Pixel>
        ImageView<const Pixel, ViewPixel> View() const
        {
            return ImageView<const Pixel, ViewPixel>(this->data.data(), this->width, this->height, this->stride);
        }
    };
} }
