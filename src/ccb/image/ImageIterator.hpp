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
#include <type_traits>

#include <ccb/image/ImageFormat.hpp>

namespace ccb { namespace image
{
    namespace details
    {
        template<typename T, typename U, typename Enable = void>
        struct CopyConstness
        {
        };

        template<typename T, typename U>
        struct CopyConstness<T, U, typename std::enable_if<std::is_const<U>::value, void>::type>
        {
            using type = typename std::add_const<T>::type;
        };

        template<typename T, typename U>
        struct CopyConstness<T, U, typename std::enable_if<!std::is_const<U>::value, void>::type>
        {
            using type = T;
        };
    }

    template<typename InPixelType>
    class ImageIterator
    {
    public:

        using PixelType = InPixelType;

        using ValueType = typename details::CopyConstness<typename PixelType::ValueType, PixelType>::type;

        using RawType = typename std::conditional<std::is_const<PixelType>::value, const void*, void*>::type;

        using ByteType = typename std::conditional<std::is_const<PixelType>::value, const uint8_t*, uint8_t*>::type;

    private:

        ValueType* data;

    public:

        ImageIterator(RawType data)
            : data(reinterpret_cast<ValueType*>(data))
        {
        }

    public:

        ValueType& operator * ()
        {
            return *this->data;
        }

        ImageIterator<PixelType> operator ++ (int)
        {
            ImageIterator<PixelType> result(this->data);

            this->data++;

            return result;
        }

        ImageIterator<PixelType>& operator ++ ()
        {
            this->data++;

            return *this;
        }

        friend inline ImageIterator<PixelType> operator + (ImageIterator<PixelType> iter, size_t advance)
        {
            return ImageIterator<PixelType>(iter.data + advance);
        }

        friend inline bool operator == (ImageIterator<PixelType> i1, ImageIterator<PixelType> i2)
        {
            return (i1.data == i2.data);
        }

        friend inline bool operator != (ImageIterator<PixelType> i1, ImageIterator<PixelType> i2)
        {
            return (i1.data != i2.data);
        }
    };

    template<typename Channel>
    class ImageIterator<CompositePixel<bool, Channel>>
    {
    public:

        using PixelType = CompositePixel<bool, Channel>;

        using ValueType = typename CompositePixel<bool, Channel>::ValueType;

        using RawType = typename std::conditional<std::is_const<PixelType>::value, const void*, void*>::type;

        using ByteType = typename std::conditional<std::is_const<PixelType>::value, const uint8_t*, uint8_t*>::type;

    private:

        ByteType data;

        size_t bitOffset;

        ValueType value;

    public:

        ImageIterator(RawType data, size_t bitOffset = 0)
            : data(reinterpret_cast<ByteType>(data))
            , bitOffset(bitOffset)
        {
            this->Read();
        }

    public:

        ValueType& operator * ()
        {
            return this->value;
        }

        ImageIterator<PixelType> operator ++ (int)
        {
            this->Write();

            ImageIterator<PixelType> result(this->data, this->bitOffset);

            if (++this->bitOffset == 8)
            {
                this->data++;
                this->bitOffset = 0;
            }

            this->Read();

            return result;
        }

        ImageIterator<PixelType>& operator ++ ()
        {
            this->Write();

            if (++this->bitOffset == 8)
            {
                this->data++;
                this->bitOffset = 0;
            }

            this->Read();

            return *this;
        }

        friend inline ImageIterator<PixelType> operator + (ImageIterator<PixelType> iter, size_t advance)
        {
            auto data = iter.data;
            auto bitOffset = iter.bitOffset;

            if (advance > (8 - bitOffset))
            {
                advance -= (8 - bitOffset);

                data += 1 + (advance >> 3);

                bitOffset = advance & 0x7;
            }
            else
            {
                bitOffset += advance;
            }

            return ImageIterator<PixelType>(data, bitOffset);
        }

        friend inline bool operator == (ImageIterator<PixelType> i1, ImageIterator<PixelType> i2)
        {
            return (i1.data == i2.data) && (i1.bitOffset == i2.bitOffset);
        }

        friend inline bool operator != (ImageIterator<PixelType> i1, ImageIterator<PixelType> i2)
        {
            return (i1.data != i2.data) || (i1.bitOffset != i2.bitOffset);
        }

    private:

        void Read()
        {
            this->value = { ((*this->data) & (1 << this->bitOffset)) != 0 };
        }

        void Write()
        {
            *this->data = (*this->data) & (~(1 << this->bitOffset)) | ((this->value[0] ? 1 : 0) << this->bitOffset);
        }
    };

    template<typename Channel>
    class ImageIterator<const CompositePixel<bool, Channel>>
    {
    public:

        using PixelType = const CompositePixel<bool, Channel>;

        using ValueType = const typename CompositePixel<bool, Channel>::ValueType;

        using RawType = const void*;

        using ByteType = const uint8_t*;

    private:

        ByteType data;

        size_t bitOffset;

    public:

        ImageIterator(RawType data, size_t bitOffset = 0)
            : data(reinterpret_cast<ByteType>(data))
            , bitOffset(bitOffset)
        {
        }

    public:

        ValueType operator * ()
        {
            return ValueType { ((*this->data) & (1 << this->bitOffset)) != 0 };
        }

        ImageIterator<PixelType> operator ++ (int)
        {
            ImageIterator<PixelType> result(this->data, this->bitOffset);

            if (++this->bitOffset == 8)
            {
                this->data++;
                this->bitOffset = 0;
            }

            return result;
        }

        ImageIterator<PixelType>& operator ++ ()
        {
            if (++this->bitOffset == 8)
            {
                this->data++;
                this->bitOffset = 0;
            }

            return *this;
        }

        friend inline ImageIterator<PixelType> operator + (ImageIterator<PixelType> iter, size_t advance)
        {
            auto data = iter.data;
            auto bitOffset = iter.bitOffset;

            if (advance > (8 - bitOffset))
            {
                advance -= (8 - bitOffset);

                data += 1 + (advance >> 3);

                bitOffset = advance & 0x7;
            }
            else
            {
                bitOffset += advance;
            }

            return ImageIterator<PixelType>(data, bitOffset);
        }

        friend inline bool operator == (ImageIterator<PixelType> i1, ImageIterator<PixelType> i2)
        {
            return (i1.data == i2.data) && (i1.bitOffset == i2.bitOffset);
        }

        friend inline bool operator != (ImageIterator<PixelType> i1, ImageIterator<PixelType> i2)
        {
            return (i1.data != i2.data) || (i1.bitOffset != i2.bitOffset);
        }
    };

    template<typename SrcIter, typename PixelType, typename Enable = void>
    class ImageViewIterator
    {
    public:

        using ValueType = typename PixelType::ValueType;

        using SrcPixelType = typename SrcIter::PixelType;

    private:

        SrcIter pos;

        ValueType pixel;

        bool pixelValid = false;

    public:

        ImageViewIterator(SrcIter pos)
            : pos(pos)
        {
        }

        ~ImageViewIterator()
        {
            this->Write();
        }

    private:

        ImageViewIterator(SrcIter pos, ValueType pixel)
            : pos(pos)
            , pixel(pixel)
        {
        }

    public:

        ValueType& operator * ()
        {
            this->Read();

            return this->pixel;
        }

        ImageViewIterator<SrcIter, PixelType> operator ++ (int)
        {
            this->Write();

            ImageViewIterator<SrcIter, PixelType> result(this->pos, this->pixel);

            this->Next();

            return result;
        }

        ImageViewIterator<SrcIter, PixelType>& operator ++ ()
        {
            this->Write();

            this->Next();

            return *this;
        }

        friend inline ImageViewIterator<SrcIter, PixelType> operator + (ImageViewIterator<SrcIter, PixelType> iter, size_t advance)
        {
            return ImageViewIterator<SrcIter, PixelType>(iter.pos + advance);
        }

        friend inline bool operator == (ImageViewIterator<SrcIter, PixelType> i1, ImageViewIterator<SrcIter, PixelType> i2)
        {
            return i1.pos == i2.pos;
        }

        friend inline bool operator != (ImageViewIterator<SrcIter, PixelType> i1, ImageViewIterator<SrcIter, PixelType> i2)
        {
            return i1.pos != i2.pos;
        }

    private:

        void Read()
        {
            if (this->pixelValid)
            {
                return;
            }

            PixelConverter<SrcPixelType, PixelType>()(this->pixel, *this->pos);

            this->pixelValid = true;
        }

        void Write()
        {
            if (!this->pixelValid)
            {
                return;
            }

            PixelConverter<PixelType, SrcPixelType>()(*this->pos, this->pixel);
        }

        void Next()
        {
            this->pos++;
            this->pixelValid = false;
        }
    };

    template<typename SrcIter, typename PixelType>
    class ImageViewIterator<SrcIter, PixelType, typename std::enable_if<std::is_const<typename SrcIter::PixelType>::value, void>::type>
    {
    public:

        using ValueType = typename PixelType::ValueType;

        using SrcPixelType = typename SrcIter::PixelType;

        using Converter = PixelConverter<
            typename std::remove_const<SrcPixelType>::type,
            typename std::remove_const<PixelType>::type>;

    private:

        SrcIter pos;

    public:

        ImageViewIterator(SrcIter pos)
            : pos(pos)
        {
        }

    public:

        ValueType operator * ()
        {
            ValueType result;
            Converter()(result, *this->pos);
            return result;
        }

        ImageViewIterator<SrcIter, PixelType> operator ++ (int)
        {
            ImageViewIterator<SrcIter, PixelType> result(this->pos);

            this->pos++;

            return result;
        }

        ImageViewIterator<SrcIter, PixelType>& operator ++ ()
        {
            this->pos++;

            return *this;
        }

        friend inline ImageViewIterator<SrcIter, PixelType> operator + (ImageViewIterator<SrcIter, PixelType> iter, size_t advance)
        {
            return ImageViewIterator<SrcIter, PixelType>(iter.pos + advance);
        }

        friend inline bool operator == (ImageViewIterator<SrcIter, PixelType> i1, ImageViewIterator<SrcIter, PixelType> i2)
        {
            return i1.pos == i2.pos;
        }

        friend inline bool operator != (ImageViewIterator<SrcIter, PixelType> i1, ImageViewIterator<SrcIter, PixelType> i2)
        {
            return i1.pos != i2.pos;
        }
    };
} }

