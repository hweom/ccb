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

#include <ccb/image/AnyImageView.hpp>
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

        template<typename PixelType, typename ImagePixelType>
        struct AnyImageFunctors
        {
            using RawType = typename std::conditional<std::is_const<PixelType>::value, const void *, void *>::type;
            using ByteType = typename std::conditional<std::is_const<PixelType>::value, const uint8_t *, uint8_t *>::type;
            using ValueType = typename std::remove_const<typename PixelType::ValueType>::type;
            using ImageValueType = typename std::remove_const<typename ImagePixelType::ValueType>::type;

            std::function<AnyImageState (RawType)> GetStart() const
            {
                return std::function<AnyImageState (RawType)>([](RawType ptr)
                {
                    return AnyImageState { { reinterpret_cast<size_t>(ptr) } };
                });
            }

            std::function<void(AnyImageState&, size_t)> GetAdvance() const
            {
                return std::function<void(AnyImageState&, size_t)>([](AnyImageState& state, size_t steps)
                {
                    return AnyImageState { { state.data[0] + steps * sizeof(typename ImagePixelType::ValueType) } };
                });
            }

            std::function<ValueType (const AnyImageState&)> GetRead() const
            {
                return std::function<ValueType (const AnyImageState&)>([](const AnyImageState& state)
                {
                    ValueType result;
                    PixelConverter<ImagePixelType, PixelType>()(result, *reinterpret_cast<const ImageValueType*>(state.data[0]));
                    return result;
                });
            }

            std::function<void (AnyImageState&, ValueType)> GetWrite() const
            {
                return std::function<void (AnyImageState&, ValueType)>([](AnyImageState& state, ValueType v)
                {
                     PixelConverter<PixelType, ImagePixelType>()(*reinterpret_cast<ImageValueType*>(state.data[0]), v);
                });
            }
        };

        template<typename PixelType, typename ImagePixelType>
        struct AnyImageViewConstructor
        {
            AnyImageView<PixelType> operator () (
                unsigned typeCode,
                typename std::conditional<std::is_const<PixelType>::value, const void*, void*>::type data,
                size_t width,
                size_t height,
                size_t stride)
            {
                if (meta::Find<ImagePixelType, PixelDataTypes>::value == typeCode)
                {
                    AnyImageFunctors<PixelType, ImagePixelType> funcs;
                    return AnyImageView<PixelType>(data, width, height, stride, funcs.GetStart(), funcs.GetAdvance(), funcs.GetRead(), funcs.GetWrite());
                }
                else if (meta::Find<ImagePixelType, PixelDataTypes>::value == meta::Npos::value)
                {
                    throw std::logic_error("Image type not registered");
                }
                else
                {
                    AnyImageViewConstructor<PixelType, typename meta::Next<ImagePixelType, PixelDataTypes>::type>()(typeCode, data, width, height, stride);
                }
            }
        };

        template<typename PixelType>
        struct AnyImageViewConstructor<PixelType, typename meta::Back<PixelDataTypes>::type>
        {
            using ImagePixelType = typename meta::Back<PixelDataTypes>::type;

            AnyImageView<PixelType> operator () (
                unsigned typeCode,
                typename std::conditional<std::is_const<PixelType>::value, const void*, void*>::type data,
                size_t width,
                size_t height,
                size_t stride)
            {
                if (meta::Find<ImagePixelType, PixelDataTypes>::value == typeCode)
                {
                    AnyImageFunctors<PixelType, ImagePixelType> funcs;
                    return AnyImageView<PixelType>(data, width, height, stride, funcs.GetStart(), funcs.GetAdvance(), funcs.GetRead(), funcs.GetWrite());
                }
                else if (meta::Find<ImagePixelType, PixelDataTypes>::value == meta::Npos::value)
                {
                    throw std::logic_error("Image type not registered");
                }
            }
        };
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

        /// Get image view.
        template<typename ViewPixel>
        AnyImageView<ViewPixel> View()
        {
            return details::AnyImageViewConstructor<ViewPixel, typename meta::Front<details::PixelDataTypes>::type>()(
                this->typeCode,
                this->data.data(),
                this->width,
                this->height,
                this->stride);
        }

        /// Get const image view.
        template<typename ViewPixel>
        AnyImageView<ViewPixel> View() const
        {
            return details::AnyImageViewConstructor<const ViewPixel, const typename meta::Front<details::PixelDataTypes>::type>()(
                this->typeCode,
                this->data.data(),
                this->width,
                this->height,
                this->stride);
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
} }
