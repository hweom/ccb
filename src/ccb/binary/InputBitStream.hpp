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

#include <type_traits>

namespace ccb { namespace binary
{
    template<typename T, typename Iter>
    class InputBitStream
    {
    private:

        using Bits = typename std::make_unsigned<T>::type;

        static const Bits BIT_MASK = 1 << (sizeof(T) * 8 - 1);

        Bits value;

        int bitCount;

        Iter cur;

        Iter end;

    public:

        InputBitStream(Iter begin, Iter end)
            : cur(begin)
            , end(end)
        {
            this->ResetValue();
        }

    public:

        bool IsGood() const
        {
            return this->cur != this->end;
        }

        operator bool () const
        {
            return this->IsGood();
        }

        bool Read()
        {
            bool result = (this->value & BIT_MASK) == BIT_MASK;

            this->value <<= 1;
            this->bitCount--;

            if (this->bitCount == 0)
            {
                this->Next();
            }

            return result;
        }

        InputBitStream<T, Iter>& operator >> (bool& value)
        {
            value = this->Read();

            return *this;
        }

    private:

        void Next()
        {
            this->cur++;

            this->ResetValue();
        }

        void ResetValue()
        {
            if (this->cur != this->end)
            {
                this->value = static_cast<Bits>(*this->cur);
            }

            this->bitCount = sizeof(T) * 8;
        }
    };

    template<typename Iter>
    InputBitStream<typename std::iterator_traits<Iter>::value_type, Iter> MakeInputBitStream(Iter begin, Iter end)
    {
        return InputBitStream<typename std::iterator_traits<Iter>::value_type, Iter>(begin, end);
    }
} }
