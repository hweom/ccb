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

#include <unordered_map>

namespace ccb { namespace binary
{
    namespace details
    {
        struct Code
        {
            uint32_t bits;

            uint32_t length;
        };

        inline bool operator == (const Code& c1, const Code& c2)
        {
            return (c1.bits == c2.bits) && (c1.length == c2.length);
        }

        inline Code& operator << (Code& c, bool bit)
        {
            c.length++;
            c.bits <<= 1;

            if (bit)
            {
                c.bits |= 1;
            }

            return c;
        }
    }
} }

namespace std
{
    template<>
    struct hash<ccb::binary::details::Code>
    {
        size_t operator () (const ccb::binary::details::Code& c) const
        {
            return (sizeof(size_t) >= sizeof(uint32_t) * 2)
                ? (static_cast<size_t>(c.bits) << 8) | c.length
                : c.bits ^ c.length;
        }
    };
}

namespace ccb { namespace binary
{
    template <typename T>
    class HuffmanDecoder
    {
    private:

        std::unordered_map<details::Code, T> table;

        uint32_t maxLength = 0;

    public:

        HuffmanDecoder(std::initializer_list<std::pair<const char*, T>> table)
        {
            for (const auto& pair : table)
            {
                this->Add(pair.first, pair.second);
            }
        }

    public:

        void Add(const char* bits, const T& value)
        {
            auto code = this->CodeFromString(bits);

            if (this->table.count(code) > 0)
            {
                throw std::logic_error("Duplicate code: " + std::string(bits));
            }

            this->table[code] = value;

            this->maxLength = std::max(this->maxLength, code.length);
        }

        template<typename BitStream>
        T Next(BitStream& bitstream) const
        {
            details::Code code = { 0, 0 };

            while ((code.length <= this->maxLength) && (bitstream))
            {
                bool bit;
                bitstream >> bit;

                code << bit;

                auto pos = this->table.find(code);
                if (pos != this->table.end())
                {
                    return pos->second;
                }
            }

            throw std::runtime_error("Sequence not recognized");
        }

    private:

        details::Code CodeFromString(const char* str)
        {
            uint32_t bits = 0;
            uint32_t length = 0;

            while (*str != 0)
            {
                length++;
                bits <<= 1;

                if (*str == '1')
                {
                    bits |= 1;
                }
                else if (*str == '0')
                {
                }
                else
                {
                    throw std::logic_error("Code string must contain only '1' and '0'");
                }

                str++;
            }

            if (length == 0)
            {
                throw std::logic_error("Code string is empty");
            }

            return details::Code { bits, length };
        }

    };
} }
