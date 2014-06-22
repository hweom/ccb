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
#include <string>
#include <sstream>
#include <type_traits>
#include <vector>

namespace ccb { namespace binary
{
    class OutputBinaryStream
    {
    private:

        std::vector<uint8_t> bytes;

    public:

        std::string GetHex() const
        {
            std::ostringstream stream;
            for (auto b : this->bytes)
            {
                stream << std::hex << std::setw(2) << std::setfill('0') << (int)b;
            }

            return stream.str();
        }

        template<typename T>
        friend typename std::enable_if<std::is_arithmetic<T>::value, OutputBinaryStream&>::type
        operator << (OutputBinaryStream& stream, T value)
        {
            auto ptr = reinterpret_cast<uint8_t*>(&value);
            for (size_t i = 0; i < sizeof(T); i++)
            {
                stream.bytes.push_back(*ptr);
                ptr++;
            }
        }
    };
} }
