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
#include <istream>
#include <ostream>
#include <stdexcept>
#include <streambuf>

namespace ccb { namespace stream
{
    template<typename T>
    class MemoryStreambuf : public std::basic_streambuf<T>
    {
    private:

        const T* readStart;

        const T* readEnd;

        const T* readPtr;

        T* writeStart = nullptr;

        T* writeEnd = nullptr;

        T* writePtr = nullptr;
    public:

        MemoryStreambuf(T* data, size_t length)
            : readStart(data)
            , readEnd(data + length)
            , readPtr(data)
            , writeStart(data)
            , writeEnd(data + length)
            , writePtr(data)
        {
        }

        MemoryStreambuf(const T* data, size_t length)
            : readStart(data)
            , readEnd(data + length)
            , readPtr(data)
        {
        }

    protected:

        virtual typename std::basic_streambuf<T>::pos_type seekoff(typename std::basic_streambuf<T>::off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which) override
        {
            if (which & std::ios_base::in)
            {
                if (dir == std::ios_base::beg)
                {
                    this->readPtr = this->readStart + off;
                }
                else if (dir == std::ios_base::cur)
                {
                    this->readPtr += off;
                }
                else
                {
                    this->readPtr = this->readEnd + off;
                }

                this->readPtr = std::min(this->readEnd, std::max(this->readStart, this->readPtr));
            }

            if (which & std::ios_base::out)
            {
                if (this->writeStart == nullptr)
                {
                    throw std::logic_error("Cannot write-seek in read-only buffer");
                }

                if (dir == std::ios_base::beg)
                {
                    this->writePtr = this->writeStart + off;
                }
                else if (dir == std::ios_base::cur)
                {
                    this->writePtr += off;
                }
                else
                {
                    this->writePtr = this->writeEnd + off;
                }

                this->writePtr = std::min(this->writeEnd, std::max(this->writeStart, this->writePtr));
            }

            return (which & std::ios_base::out)
                ? std::distance(this->writeStart, this->writePtr)
                : std::distance(this->readStart, this->readPtr);
        }

        virtual typename std::basic_streambuf<T>::pos_type seekpos(typename std::basic_streambuf<T>::pos_type off, std::ios_base::openmode which) override
        {
            return this->seekoff(off, std::ios_base::beg, which);
        }

        virtual std::streamsize showmanyc() override
        {
            assert (std::less_equal<const T *>()(readPtr, readEnd));

            return this->readEnd - this->readPtr;
        }

        virtual std::streamsize xsgetn(typename std::basic_streambuf<T>::char_type* s, std::streamsize n) override
        {
            auto len = std::min(static_cast<size_t>(n), static_cast<size_t>(this->readEnd - this->readPtr));

            memcpy(s, this->readPtr, len);

            this->readPtr += len;

            return len;
        }

        virtual typename std::basic_streambuf<T>::int_type underflow() override
        {
            if (this->readPtr >= this->readEnd)
            {
                return std::basic_streambuf<T>::traits_type::eof();
            }

            return std::basic_streambuf<T>::traits_type::to_int_type(*this->readPtr);
        }

        virtual typename std::basic_streambuf<T>::int_type uflow() override
        {
            if (this->readPtr >= this->readEnd)
            {
                return std::basic_streambuf<T>::traits_type::eof();
            }

            return std::basic_streambuf<T>::traits_type::to_int_type(*this->readPtr++);
        }

        virtual typename std::basic_streambuf<T>::int_type pbackfail(typename std::basic_streambuf<T>::int_type ch) override
        {
            if ((this->readPtr == this->readStart) ||
                (ch != std::basic_streambuf<T>::traits_type::eof()) && (ch != this->readPtr[-1]))
            {
                return std::basic_streambuf<T>::traits_type::eof();
            }

            return std::basic_streambuf<T>::traits_type::to_int_type(*--this->readPtr);
        }

        virtual std::streamsize xsputn(const typename std::basic_streambuf<T>::char_type* s, std::streamsize n) override
        {
            if (this->writeStart == nullptr)
            {
                throw std::logic_error("Cannot write into read-only buffer");
            }

            auto len = std::min(static_cast<size_t>(n), static_cast<size_t>(this->writeEnd - this->writePtr));

            memcpy(this->writePtr, s, len);

            this->writePtr += len;

            return len;
        }

        virtual typename std::basic_streambuf<T>::int_type overflow(typename std::basic_streambuf<T>::int_type c) override
        {
            if (this->writePtr >= this->writeEnd)
            {
                return std::basic_streambuf<T>::traits_type::eof();
            }

            *this->writePtr = static_cast<typename std::basic_streambuf<T>::char_type>(c);

            return c;
        }
    };

    template<typename T>
    class MemoryIStream : public std::basic_istream<T>
    {
    private:

        MemoryStreambuf<T> streambuf;

    public:
#ifdef _MSC_VER
        MemoryIStream(const T* data, size_t length)
            : std::basic_istream<T>(std::_Noinit)
            , streambuf(data, length)
#else
        MemoryIStream(const T* data, size_t length)
            : streambuf(data, length)
#endif
        {
            this->init(&this->streambuf);
        }
    };
} }
