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
#include <ccb/image/ImageAlgorithm.hpp>

#define IMAGE_REGISTER_LAYOUT(Layout) Layout<bool>, Layout<uint8_t>

namespace ccb { namespace image
{
    namespace details
    {
        template<typename T, typename... Types>
        struct First
        {
            using type = T;
        };

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
                    state.data[0] += steps * sizeof(typename ImagePixelType::ValueType);
                });
            }

            std::function<ValueType (const AnyImageState&)> GetRead() const
            {
                return std::function<ValueType (const AnyImageState&)>([](const AnyImageState& state)
                {
                    ValueType result;
                    PixelConverter<typename std::remove_const<ImagePixelType>::type, typename std::remove_const<PixelType>::type>()(result, *reinterpret_cast<const ImageValueType*>(state.data[0]));
                    return result;
                });
            }

            std::function<void (AnyImageState&, ValueType)> GetWrite() const
            {
                return std::function<void (AnyImageState&, ValueType)>([](AnyImageState& state, ValueType v)
                {
                     PixelConverter<typename std::remove_const<PixelType>::type, typename std::remove_const<ImagePixelType>::type>()(*reinterpret_cast<ImageValueType*>(state.data[0]), v);
                });
            }
        };

        template<typename ViewPixel, typename... PixelTypes>
        struct AnyImageViewConstructor
        {
            AnyImageView<ViewPixel> operator () (
                unsigned typeCode,
                typename std::conditional<std::is_const<ViewPixel>::value, const void*, void*>::type data,
                size_t width,
                size_t height,
                size_t stride)
            {
                throw std::logic_error("Image type not registered");
            }
        };

        template<typename ViewPixel, typename First, typename... PixelTypes>
        struct AnyImageViewConstructor<ViewPixel, First, PixelTypes...>
        {
            AnyImageView<ViewPixel> operator () (
                unsigned typeCode,
                typename std::conditional<std::is_const<ViewPixel>::value, const void*, void*>::type data,
                size_t width,
                size_t height,
                size_t stride)
            {
                if (typeCode == 0)
                {
                    AnyImageFunctors<ViewPixel, First> funcs;
                    return AnyImageView<ViewPixel>(data, width, height, stride, funcs.GetStart(), funcs.GetAdvance(), funcs.GetRead(), funcs.GetWrite());
                }
                else
                {
                    return AnyImageViewConstructor<ViewPixel, PixelTypes...>()(typeCode - 1, data, width, height, stride);
                }
            }
        };

        struct AnyImageInfo
        {
            size_t componentCount;

            size_t bitsPerPixel;
        };

        template<typename... PixelTypes>
        struct AnyImageInfoProvider
        {
            AnyImageInfo operator () (unsigned typeCode)
            {
                throw std::logic_error("Image type not registered");
            }
        };

        template<typename T, typename... PixelTypes>
        struct AnyImageInfoProvider<T, PixelTypes...>
        {
            AnyImageInfo operator () (unsigned typeCode)
            {
                if (typeCode == 0)
                {
                    return AnyImageInfo
                    {
                        PixelTraits<T>::ComponentCount,
                        PixelTraits<T>::BitsPerPixel
                    };
                }
                else
                {
                    return AnyImageInfoProvider<PixelTypes...>()(typeCode - 1);
                }
            }
        };
    }

    template<typename... PixelTypes>
    class AnyImage
    {
    private:

        std::vector<uint8_t> data;

        size_t width = 0;

        size_t height = 0;

        size_t stride = 0;

        unsigned typeCode = 0;

    public:

        AnyImage()
        {
        }

        AnyImage(AnyImage&& other)
            : data(std::move(other.data))
            , width(other.width)
            , height(other.height)
            , stride(other.stride)
            , typeCode(other.typeCode)
        {
        }

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

        size_t GetComponentCount() const
        {
            return details::AnyImageInfoProvider<PixelTypes...>()(this->typeCode).componentCount;
        }

        size_t GetBitsPerPixel() const
        {
            return details::AnyImageInfoProvider<PixelTypes...>()(this->typeCode).bitsPerPixel;
        }

        uint8_t* GetData()
        {
            return this->data.data();
        }

        const uint8_t* GetData() const
        {
            return this->data.data();
        }

        template<typename Pixel>
        Image<Pixel> ReinterpretAs()
        {
            return Image<Pixel>(std::move(this->data), width, height, stride);
        }

        /// Get image view.
        template<typename SrcPixel, typename ViewPixel = SrcPixel>
        AnyImageView<ViewPixel> ViewAs()
        {
            details::AnyImageFunctors<ViewPixel, SrcPixel> funcs;
            return AnyImageView<ViewPixel>(
                this->data.data(),
                this->width,
                this->height,
                this->stride,
                funcs.GetStart(),
                funcs.GetAdvance(),
                funcs.GetRead(),
                funcs.GetWrite());
        }

        /// Get const image view.
        template<typename SrcPixel, typename ViewPixel = SrcPixel>
        AnyImageView<const ViewPixel> ViewAs() const
        {
            details::AnyImageFunctors<const ViewPixel, SrcPixel> funcs;
            return AnyImageView<const ViewPixel>(
                this->data.data(),
                this->width,
                this->height,
                this->stride,
                funcs.GetStart(),
                funcs.GetAdvance(),
                funcs.GetRead(),
                funcs.GetWrite());
        }

        /// Get image view.
        template<typename ViewPixel>
        AnyImageView<ViewPixel> View()
        {
            return details::AnyImageViewConstructor<ViewPixel, PixelTypes...>()(
                this->typeCode,
                this->data.data(),
                this->width,
                this->height,
                this->stride);
        }

        /// Get const image view.
        template<typename ViewPixel>
        AnyImageView<const ViewPixel> View() const
        {
            return details::AnyImageViewConstructor<const ViewPixel, PixelTypes...>()(
                this->typeCode,
                this->data.data(),
                this->width,
                this->height,
                this->stride);
        }

        template<typename PixelType>
        Image<PixelType> Convert() const
        {
            Image<PixelType> image(this->width, this->height);

            Copy(image.View(), this->View<const PixelType>());

            return image;
        }

        template<typename PixelType>
        bool IsOfType() const
        {
            return details::Offset<PixelType, PixelTypes...>::value == this->typeCode;
        }

    public:

        template<typename Pixel>
        static AnyImage Create(size_t width, size_t height)
        {
            auto bpp = PixelTraits<Pixel>::BitsPerPixel;
            auto stride = (((width * bpp + 7) / 8 + 3) / 4) * 4;
            auto typeCode = details::Offset<Pixel, PixelTypes...>::value;
            if (typeCode == details::Npos::value)
            {
                throw std::logic_error("Unregistered pixel type");
            }

            return AnyImage(std::vector<uint8_t>(stride * height), width, height, stride, typeCode);
        }

        template<typename Pixel>
        static AnyImage Create(std::vector<uint8_t>&& data, size_t width, size_t height, size_t stride)
        {
            auto typeCode = details::Offset<Pixel, PixelTypes...>::value;
            if (typeCode == details::Npos::value)
            {
                throw std::logic_error("Unregistered pixel type");
            }

            return AnyImage(std::move(data), width, height, stride, typeCode);
        }
    };
} }
