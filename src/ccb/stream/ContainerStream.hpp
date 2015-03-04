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
#include <stdexcept>
#include <streambuf>

namespace ccb { namespace stream
{
    template<typename T, typename C>
    class ContainerStreambuf : public std::basic_streambuf<T>
    {
    private:

        typedef typename C::iterator Iter;

        typedef typename C::const_iterator ConstIter;

        C& container;

        ConstIter readPtr;

        Iter writePtr;

    public:

        template<typename U = C>
        ContainerStreambuf(typename std::enable_if<std::is_const<U>::value, U>::type& container)
            : container(container)
            , readPtr(container.begin())
        {
        }

        template<typename U = C>
        ContainerStreambuf(typename std::enable_if<!std::is_const<U>::value, U>::type& container)
            : container(container)
            , readPtr(container.begin())
            , writePtr(container.begin())
        {
        }

    protected:

        virtual typename std::basic_streambuf<T>::pos_type seekoff(typename std::basic_streambuf<T>::off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which) override
        {
            return this->seekpos(this->ToGlobalOffset(off, dir, this->readPtr), which);
        }

        virtual typename std::basic_streambuf<T>::pos_type seekpos(typename std::basic_streambuf<T>::pos_type off, std::ios_base::openmode which) override
        {
            assert (off >= 0);

            size_t offset = static_cast<size_t>(std::max(typename std::basic_streambuf<T>::pos_type(0), off));

            if (which & std::ios_base::out)
            {
                if (std::is_const<C>::value)
                {
                    throw std::logic_error("Cannot write-seek in read-only buffer");
                }

                auto readPos = std::distance(this->CBegin(), this->readPtr);

                while (this->container.size() < offset)
                {
                    this->container.insert(this->container.end(), typename C::value_type());
                }

                this->readPtr = this->container.begin();
                std::advance(this->readPtr, readPos);

                this->writePtr = this->container.begin();
                std::advance(this->writePtr, offset);
            }

            if (which & std::ios_base::in)
            {
                this->readPtr = this->container.begin();
                std::advance(this->readPtr, offset);
            }

            return (which & std::ios_base::out)
                ? std::distance(this->container.begin(), this->writePtr)
                : std::distance(this->CBegin(), this->readPtr);
        }

        virtual std::streamsize showmanyc() override
        {
            return std::distance(this->readPtr, this->CEnd());
        }

        virtual std::streamsize xsgetn(typename std::basic_streambuf<T>::char_type* s, std::streamsize n) override
        {
            auto endCopyPtr = this->readPtr;
            std::advance(endCopyPtr, n);

            std::copy(this->readPtr, endCopyPtr, s);

            auto len = std::distance(this->readPtr, endCopyPtr);

            this->readPtr = endCopyPtr;

            return len;
        }

        virtual typename std::basic_streambuf<T>::int_type underflow() override
        {
            if (this->readPtr == this->container.end())
            {
                return std::basic_streambuf<T>::traits_type::eof();
            }

            return std::basic_streambuf<T>::traits_type::to_int_type(*this->readPtr);
        }

        virtual typename std::basic_streambuf<T>::int_type uflow() override
        {
            if (this->readPtr == this->container.end())
            {
                return std::basic_streambuf<T>::traits_type::eof();
            }

            return std::basic_streambuf<T>::traits_type::to_int_type(*this->readPtr++);
        }

        virtual typename std::basic_streambuf<T>::int_type pbackfail(typename std::basic_streambuf<T>::int_type ch) override
        {
            if (this->readPtr == this->container.begin())
            {
                return std::basic_streambuf<T>::traits_type::eof();
            }

            auto prev = this->readPtr;
            prev--;

            if ((ch != std::basic_streambuf<T>::traits_type::eof()) && (ch != *prev))
            {
                return std::basic_streambuf<T>::traits_type::eof();
            }

            return std::basic_streambuf<T>::traits_type::to_int_type(*--this->readPtr);
        }

        virtual std::streamsize xsputn(const typename std::basic_streambuf<T>::char_type* s, std::streamsize n) override
        {
            if (std::is_const<C>::value)
            {
                throw std::logic_error("Cannot write into read-only buffer");
            }

            auto len = std::min(n, std::distance(this->writePtr, this->container.end()));

            auto endPtr = this->writePtr;
            std::advance(endPtr, len);

            std::copy(s, s + len, this->writePtr);

            if (n > len)
            {
                auto readPos = std::distance(this->CBegin(), this->readPtr);

                std::copy(s + len, s + n, std::back_inserter(this->container));
                this->writePtr = this->container.end();

                this->readPtr = this->container.begin();
                std::advance(this->readPtr, readPos);
            }
            else
            {
                this->writePtr = endPtr;
            }

            assert (this->readPtr >= this->container.begin());

            return n;
        }

        virtual typename std::basic_streambuf<T>::int_type overflow(typename std::basic_streambuf<T>::int_type c) override
        {
            assert (this->readPtr >= this->container.begin());

            if (this->writePtr == this->container.end())
            {
                auto readPos = std::distance(this->CBegin(), this->readPtr);

                this->writePtr = this->container.insert(this->writePtr, static_cast<typename C::value_type>(c));

                this->readPtr = this->container.begin();
                std::advance(this->readPtr, readPos);
            }
            else
            {
                *this->writePtr = static_cast<typename C::value_type>(c);
            }

            assert (this->readPtr >= this->container.begin());

            return c;
        }

    private:

        typename std::basic_streambuf<T>::off_type ToGlobalOffset(
            typename std::basic_streambuf<T>::off_type off,
            std::ios_base::seekdir dir,
            ConstIter cur)
        {
            if (dir == std::ios_base::beg)
            {
                return off;
            }
            else if (dir == std::ios_base::cur)
            {
                return std::distance(this->CBegin(), cur) + off;
            }
            else
            {
                return this->container.size() + off;
            }
        }

        ConstIter CBegin() const
        {
            return static_cast<const C&>(this->container).begin();
        }

        ConstIter CEnd() const
        {
            return static_cast<const C&>(this->container).end();
        }

    };

    template<typename T, typename C>
    class ContainerOStream : public std::basic_ostream<T>
    {
    private:

        ContainerStreambuf<T, C> streambuf;

    public:

        ContainerOStream(C& container)
            : streambuf(container)
        {
            this->init(&this->streambuf);
        }
    };
} }
