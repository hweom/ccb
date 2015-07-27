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

    using Yuv8 = CompositePixel<uint8_t, Luminance, ChromaBlue, ChromaRed>;
    using Yuv16 = CompositePixel<uint16_t, Luminance, ChromaBlue, ChromaRed>;

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
        struct Offset : std::integral_constant<size_t, 0> {};

        template<typename T, typename L1, typename... L>
        struct Offset<T, L1, L...>
            : std::conditional<std::is_same<T, L1>::value, std::integral_constant<size_t, 0>, std::integral_constant<size_t, Offset<T, L...>::value + 1>>::type {};

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
        static const size_t BitsPerPixel = sizeof...(Channels) * sizeof(Type) * 8;
    };

    template<typename... Channels>
    struct PixelTraits<CompositePixel<bool, Channels...>>
    {
        static const size_t ComponentCount = sizeof...(Channels);
        static const size_t BitsPerPixel = sizeof...(Channels);
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
    struct TypeConverter<bool, T, typename std::enable_if<!std::is_same<T, bool>::value && std::is_integral<T>::value, void>::type>
    {
        T operator () (bool v)
        {
            return v ? std::numeric_limits<T>::max() : std::numeric_limits<T>::min();
        }
    };

    template<typename T>
    struct TypeConverter<T, bool, typename std::enable_if<!std::is_same<T, bool>::value && std::is_integral<T>::value, void>::type>
    {
        bool operator () (T v)
        {
            return v >= (std::numeric_limits<T>::max() / 2 + std::numeric_limits<T>::min() / 2) ? true : false;
        }
    };

    template<typename T>
    struct TypeConverter<bool, T, typename std::enable_if<!std::is_same<T, bool>::value && std::is_floating_point<T>::value, void>::type>
    {
        T operator () (bool v)
        {
            return v ? static_cast<T>(1) : static_cast<T>(0);
        }
    };

    template<typename T>
    struct TypeConverter<T, bool, typename std::enable_if<!std::is_same<T, bool>::value && std::is_floating_point<T>::value, void>::type>
    {
        bool operator () (T v)
        {
            return v >= static_cast<T>(0.5) ? true : false;
        }
    };

    template<typename Type, typename Channel, typename SrcPixel, typename Enable = void>
    struct ChannelConverter
    {
        Type operator () (Type d, typename SrcPixel::ValueType s)
        {
            return d;
        }
    };

    template<typename SrcType, typename DstType>
    struct TypeConverter<
        SrcType,
        DstType,
        typename std::enable_if<
            std::is_floating_point<SrcType>::value &&
            std::is_integral<DstType>::value &&
            !std::is_same<DstType, bool>::value,
            void>::type>
    {
        DstType operator () (SrcType v)
        {
            return static_cast<DstType>(
                static_cast<SrcType>(std::numeric_limits<DstType>::min()) +
                    (static_cast<SrcType>(std::numeric_limits<DstType>::max()) - static_cast<SrcType>(std::numeric_limits<DstType>::min())) * v);
        }
    };

    template<typename SrcType, typename DstType>
    struct TypeConverter<
        SrcType,
        DstType,
        typename std::enable_if<
            std::is_integral<SrcType>::value &&
            std::is_floating_point<DstType>::value &&
            !std::is_same<SrcType, bool>::value,
            void>::type>
    {
        DstType operator () (SrcType v)
        {
            return (static_cast<DstType>(v) - static_cast<DstType>(std::numeric_limits<SrcType>::min())) /
                (static_cast<DstType>(std::numeric_limits<SrcType>::max()) - static_cast<DstType>(std::numeric_limits<SrcType>::min()));
        }
    };

    /// Channel converter for the case when target channel is present among source channels.
    template<typename Type, typename Channel, typename SrcType, typename... Channels>
    struct ChannelConverter<Type, Channel, CompositePixel<SrcType, Channels...>, typename std::enable_if<details::Contains<Channel, Channels...>::value, void>::type>
    {
        Type operator () (Type d, typename CompositePixel<SrcType, Channels...>::ValueType s)
        {
            return TypeConverter<SrcType, Type>()(s[details::Offset<Channel, Channels...>::value]);
        }
    };

    /// Red from YUV
    template<typename Type, typename SrcType, typename... Channels>
    struct ChannelConverter<
        Type,
        Red,
        CompositePixel<SrcType, Channels...>,
        typename std::enable_if<details::Contains<Luminance, Channels...>::value && details::Contains<ChromaRed, Channels...>::value && details::Contains<ChromaBlue, Channels...>::value, void>::type>
    {
        Type operator () (Type d, typename CompositePixel<SrcType, Channels...>::ValueType s)
        {
            return
                TypeConverter<float, Type>()(
                    TypeConverter<SrcType, float>()(s[details::Offset<Luminance, Channels...>::value]) +
                    1.402f * (TypeConverter<SrcType, float>()(s[details::Offset<ChromaRed, Channels...>::value]) - 0.5f));
        }
    };

    /// Green from YUV
    template<typename Type, typename SrcType, typename... Channels>
    struct ChannelConverter<
        Type,
        Green,
        CompositePixel<SrcType, Channels...>,
        typename std::enable_if<details::Contains<Luminance, Channels...>::value && details::Contains<ChromaRed, Channels...>::value && details::Contains<ChromaBlue, Channels...>::value, void>::type>
    {
        Type operator () (Type d, typename CompositePixel<SrcType, Channels...>::ValueType s)
        {
            return
                TypeConverter<float, Type>()(
                    TypeConverter<SrcType, float>()(s[details::Offset<Luminance, Channels...>::value]) -
                    0.34414f * (TypeConverter<SrcType, float>()(s[details::Offset<ChromaBlue, Channels...>::value]) - 0.5f) -
                    0.71414f * (TypeConverter<SrcType, float>()(s[details::Offset<ChromaRed, Channels...>::value]) - 0.5f));
        }
    };

    /// Blue from YUV
    template<typename Type, typename SrcType, typename... Channels>
    struct ChannelConverter<
        Type,
        Blue,
        CompositePixel<SrcType, Channels...>,
        typename std::enable_if<details::Contains<Luminance, Channels...>::value && details::Contains<ChromaRed, Channels...>::value && details::Contains<ChromaBlue, Channels...>::value, void>::type>
    {
        Type operator () (Type d, typename CompositePixel<SrcType, Channels...>::ValueType s)
        {
            return
                TypeConverter<float, Type>()(
                    TypeConverter<SrcType, float>()(s[details::Offset<Luminance, Channels...>::value]) +
                    1.772f * (TypeConverter<SrcType, float>()(s[details::Offset<ChromaBlue, Channels...>::value]) - 0.5f));
        }
    };


    template<typename SrcPixel, typename Type, typename... Channels>
    struct ChannelDispatcher
    {
        void operator ()(Type* values, typename SrcPixel::ValueType s)
        {
        }
    };

    template<typename SrcPixel, typename Type, typename C1, typename... Channels>
    struct ChannelDispatcher<SrcPixel, Type, C1, Channels...>
    {
        void operator ()(Type* values, typename SrcPixel::ValueType s)
        {
            *values = ChannelConverter<Type, C1, SrcPixel>()(*values, s);

            ChannelDispatcher<SrcPixel, Type, Channels...>()(values + 1, s);
        }
    };

    template<typename SrcPixel, typename Type>
    struct ChannelDispatcher<SrcPixel, Type>
    {
        void operator ()(Type* values, typename SrcPixel::ValueType s)
        {
        }
    };

    template<typename ScrPixel, typename DstPixel, typename Enable = void>
    struct PixelConverter
    {
        void operator () (typename DstPixel::ValueType& d, typename ScrPixel::ValueType s)
        {
            static_assert(sizeof(typename DstPixel::ValueType) == 0, "Unsupported pixel conversion");
        }
    };

//    /// Converter from same pixel types.
//    template<typename Pixel>
//    struct PixelConverter<Pixel, Pixel>
//    {
//        void operator () (typename Pixel::ValueType& d, typename Pixel::ValueType s)
//        {
//            d = s;
//        }
//    };

//    /// Converter from pixels having the same layout, but different unit types.
//    template<typename Type1, typename Type2, typename... Channels>
//    struct PixelConverter<CompositePixel<Type1, Channels...>, CompositePixel<Type2, Channels...>>
//    {
//        void operator () (typename CompositePixel<Type2, Channels...>::ValueType& d, typename CompositePixel<Type1, Channels...>::ValueType s)
//        {
//            for (size_t i = 0; i < sizeof...(Channels); i++)
//            {
//                d.v[i] = TypeConverter<Type1, Type2>()(s.v[i]);
//            }
//        }
//    };

    /// Per-channel converter specialization.
    template<typename SrcPixel, typename Type, typename... Channels>
    struct PixelConverter<SrcPixel, CompositePixel<Type, Channels...>>
    {
        void operator () (typename CompositePixel<Type, Channels...>::ValueType& d, typename SrcPixel::ValueType s)
        {
            ChannelDispatcher<SrcPixel, Type, Channels...>()(d.data(), s);
        }
    };
} }
