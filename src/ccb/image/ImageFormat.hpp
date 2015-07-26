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

#include <array>
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
    struct Gray {};
    struct Cyan {};
    struct Magenta {};
    struct Yellow {};
    struct Key {};
    struct Luminance {};
    struct ChromaRed {};
    struct ChromaBlue {};

    template<typename Type, typename... Channels>
    struct CompositePixel
    {
        using DataType = Type;
        using ValueType = std::array<Type, sizeof...(Channels)>;

        std::array<Type, sizeof...(Channels)> v;
    };

    using Alpha8 = CompositePixel<uint8_t, Alpha>;
    using Alpha1 = CompositePixel<bool, Alpha>;

    using Gray1 = CompositePixel<bool, Gray>;
    using Gray8 = CompositePixel<uint8_t, Gray>;
    using Gray16 = CompositePixel<uint16_t, Gray>;

    using Rgba8 = CompositePixel<uint8_t, Red, Green, Blue, Alpha>;

    using Rgb8 = CompositePixel<uint8_t, Red, Green, Blue>;
    using Rgb16 = CompositePixel<uint16_t, Red, Green, Blue>;

    using Cmyk8 = CompositePixel<uint8_t, Cyan, Magenta, Yellow, Key>;
    using Cmyk16 = CompositePixel<uint8_t, Cyan, Magenta, Yellow, Key>;

    using Yuv8 = CompositePixel<uint8_t, Luminance, ChromaRed, ChromaBlue>;
    using Yuv16 = CompositePixel<uint16_t, Luminance, ChromaRed, ChromaBlue>;

    namespace details
    {
        template<typename T, typename... L>
        struct Contains : std::false_type {};

        template<typename T, typename L1, typename... L>
        struct Contains<T, L1, L...> : std::integral_constant<
            bool,
            std::is_same<T, L1>::value || Contains<T, L...>::value> {};

        template<typename T>
        struct Contains<T> : std::false_type {};

        template<typename T, typename... L>
        struct Offset {};

        template<typename T, typename L1, typename... L>
        struct Offset<T, L1, L...>
            : std::integral_constant<size_t, std::is_same<T, L1>::value ? 0 : Offset<T, L...>::value> {};

        template<typename Channel, typename Pixel, typename Enable = void>
        struct HasChannel : std::false_type { };

        template<typename Channel, typename Type, typename... Channels>
        struct HasChannel<
            Channel,
            CompositePixel<Type, Channels...>,
            typename std::enable_if<Contains<Channel, Channels...>::value, void>::type>
            : std::true_type {};

        template<typename Channel, typename Pixel, typename Enable = void>
        struct ChannelOffset : std::integral_constant<size_t, 0xffff> {};

        template<typename Channel, typename Type, typename... Channels>
        struct ChannelOffset<
            Channel,
            CompositePixel<Type, Channels...>,
            typename std::enable_if<Contains<Channel, Channels...>::value, void>::type>
            : Offset<Channel, Channels...> {};
    }

    template<typename Pixel>
    struct PixelTraits
    {
    };

    template<typename Type, typename... Channels>
    struct PixelTraits<CompositePixel<Type, Channels...>>
    {
        static const size_t ComponentCount = sizeof...(Channels);
        static const size_t BitsPerPixel = sizeof...(Channels) * sizeof(Type);
    };

    template<typename Pixel, typename Channel, typename Enable = void>
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

    template<typename Pixel, typename Channel>
    struct PixelChannelTraits<
        Pixel,
        Channel,
        typename std::enable_if<details::HasChannel<Channel, Pixel>::value, void>::type>
    {
        static const bool HasChannel = true;

        typename Pixel::DataType Get(Pixel pixel) const
        {
            return pixel.v[details::ChannelOffset<Channel, Pixel>::value];
        }

        void Set(Pixel& pixel, typename Pixel::DataType v) const
        {
            pixel.v[details::ChannelOffset<Channel, Pixel>::value] = v;
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

    /// RGBA to Gray converter.
    template<typename SrcType, typename DstType>
    struct PixelConverter<
        RgbaPixel<SrcType>,
        MonochromePixel<Gray, DstType>>
    {
        void operator () (DstType& d, RgbaPixel<SrcType> s)
        {
            d = TypeConverter<SrcType, DstType>()(static_cast<SrcType>(0.2126f * s.red + 0.7152f * s.green + 0.0722f * s.blue));
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
