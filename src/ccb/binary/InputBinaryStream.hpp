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
#include <iomanip>
#include <stdexcept>
#include <string>
#include <sstream>
#include <type_traits>
#include <vector>

namespace ccb { namespace binary
{
    class InputBinaryStream
    {
    private:

        std::vector<uint8_t> bytes;

        size_t pos = 0;

    public:

        InputBinaryStream(std::vector<uint8_t>&& bytes)
            : bytes(bytes)
        {
        }

    public:

        bool End() const
        {
            return this->pos >= this->bytes.size();
        }

        template<typename T>
        friend typename std::enable_if<std::is_arithmetic<T>::value, InputBinaryStream&>::type
        operator >> (InputBinaryStream& stream, T& value)
        {
            auto ptr = reinterpret_cast<uint8_t*>(&value);
            for (size_t i = 0; (i < sizeof(T)) && (stream.pos < stream.bytes.size()); i++)
            {
                *(ptr++) = stream.bytes[stream.pos++];
            }

            return stream;
        }

    public:

        static InputBinaryStream FromHex(const std::string& hex)
        {
            std::vector<uint8_t> bytes;

            for (size_t i = 0; i < hex.size(); i += 2)
            {
                uint8_t v = (FromHex(hex[i]) << 4) | FromHex(hex[i+1]);
                bytes.push_back(v);
            }

            return InputBinaryStream(std::move(bytes));
        }

    private:

        static uint8_t FromHex(char c)
        {
            c = tolower(c);

            if ((c >= '0') && (c <= '9'))
            {
                return c - '0';
            }
            else if ((c >= 'a') && (c <= 'f'))
            {
                return (c - 'a') + 10;
            }
            else
            {
                throw std::invalid_argument("Not a hex symbol: " + c);
            }
        }
    };
} }
