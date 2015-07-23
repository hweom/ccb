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
#include <limits>
#include <tuple>

namespace ccb { namespace image
{
    /// Channels.
    struct Alpha {};
    struct Red {};
    struct Green {};
    struct Blue {};

    template<typename Channel, typename Type>
    struct MonochromePixel
    {
        using DataType = Type;
        using ValueType = Type;

        Type value;
    };

    template<typename Type>
    using AlphaPixel = MonochromePixel<Alpha, Type>;

    using Alpha8 = AlphaPixel<uint8_t>;
    using Alpha1 = AlphaPixel<bool>;

    template<typename Type>
    struct RgbaPixel
    {
        using DataType = Type;
        using ValueType = RgbaPixel<Type>;

        Type red;
        Type green;
        Type blue;
        Type alpha;
    };

    using Rgba8 = RgbaPixel<uint8_t>;

    template<typename Pixel>
    struct PixelTraits
    {
    };

    template<typename Type>
    struct PixelTraits<RgbaPixel<Type>>
    {
        static const size_t BitsPerPixel = sizeof(Type) * 8 * 4;
    };

    template<typename Channel, typename Type>
    struct PixelTraits<MonochromePixel<Channel, Type>>
    {
        static const size_t BitsPerPixel = sizeof(Type) * 8;
    };

    template<typename Channel>
    struct PixelTraits<MonochromePixel<Channel, bool>>
    {
        static const size_t BitsPerPixel = 1;
    };

    template<typename Pixel, typename Channel>
    struct PixelChannelTraits
    {
        static const bool HasChannel = false;

        typename Pixel::DataType Get(Pixel pixel) const
        {
            return 0;
        }

        void Set(Pixel& pixel, typename Pixel::DataType v) const
        {
        }
    };

    template<typename Type>
    struct PixelChannelTraits<RgbaPixel<Type>, Alpha>
    {
        static const bool HasChannel = true;

        Type Get(RgbaPixel<Type> pixel) const
        {
            return pixel.alpha;
        }

        void Set(RgbaPixel<Type>& pixel, Type v) const
        {
            pixel.alpha = v;
        }
    };

    template<typename Type>
    struct PixelChannelTraits<RgbaPixel<Type>, Red>
    {
        static const bool HasChannel = true;

        Type Get(RgbaPixel<Type> pixel) const
        {
            return pixel.red;
        }

        void Set(RgbaPixel<Type>& pixel, Type v) const
        {
            pixel.red = v;
        }
    };

    template<typename Type>
    struct PixelChannelTraits<RgbaPixel<Type>, Green>
    {
        static const bool HasChannel = true;

        Type Get(RgbaPixel<Type> pixel) const
        {
            return pixel.green;
        }

        void Set(RgbaPixel<Type>& pixel, Type v) const
        {
            pixel.green = v;
        }
    };

    template<typename Type>
    struct PixelChannelTraits<RgbaPixel<Type>, Blue>
    {
        static const bool HasChannel = true;

        Type Get(RgbaPixel<Type> pixel) const
        {
            return pixel.blue;
        }

        void Set(RgbaPixel<Type>& pixel, Type v) const
        {
            pixel.blue = v;
        }
    };

    template<typename SrcType, typename DstType, typename Enable = void>
    struct TypeConverter
    {
        DstType operator () (SrcType v)
        {
            static_assert(sizeof(SrcType) == 0, "Unknown type conversion");
        }
    };

    template<typename Type>
    struct TypeConverter<Type, Type>
    {
        Type operator () (Type v)
        {
            return v;
        }
    };

    template<typename T>
    struct TypeConverter<bool, T, typename std::enable_if<std::is_arithmetic<T>::value, void>::type>
    {
        T operator () (bool v)
        {
            return v ? std::numeric_limits<T>::max() : std::numeric_limits<T>::min();
        }
    };

    template<typename T>
    struct TypeConverter<T, bool, typename std::enable_if<std::is_arithmetic<T>::value, void>::type>
    {
        bool operator () (T v)
        {
            return v >= (std::numeric_limits<T>::max() / 2 + std::numeric_limits<T>::min() / 2) ? true : false;
        }
    };

    template<typename ScrPixel, typename DstPixel, typename Enable = void>
    struct PixelConverter
    {
        void operator () (typename DstPixel::ValueType& d, typename ScrPixel::ValueType s)
        {
        }
    };

    template<typename Pixel>
    struct PixelConverter<Pixel, Pixel>
    {
        void operator () (typename Pixel::ValueType& d, typename Pixel::ValueType s)
        {
            d = s;
        }
    };

    template<typename SrcPixel, typename Channel, typename Type>
    struct PixelConverter<
        SrcPixel,
        MonochromePixel<Channel, Type>,
        typename std::enable_if<PixelChannelTraits<SrcPixel, Channel>::HasChannel, void>::type>
    {
        void operator () (Type& d, typename SrcPixel::ValueType s)
        {
            d = TypeConverter<typename SrcPixel::DataType, Type>()(PixelChannelTraits<SrcPixel, Channel>().Get(s));
        }
    };

    template<typename DstPixel, typename Channel, typename Type>
    struct PixelConverter<
        MonochromePixel<Channel, Type>,
        DstPixel,
        typename std::enable_if<PixelChannelTraits<DstPixel, Channel>::HasChannel, void>::type>
    {
        void operator () (typename DstPixel::ValueType& d, Type s)
        {
            PixelChannelTraits<DstPixel, Channel>().Set(d, TypeConverter<Type, typename DstPixel::DataType>()(s));
        }
    };
} }
