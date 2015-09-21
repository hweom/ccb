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
#include <string>
#include <vector>

namespace ccb { namespace crypt
{
    /// MD5 hash implementation.
    class Md5
    {
    private:

        static const size_t BLOCK_SIZE = 64;

        static const size_t DIGEST_LEN = 16;

        static const uint32_t S11 = 7;
        static const uint32_t S12 = 12;
        static const uint32_t S13 = 17;
        static const uint32_t S14 = 22;
        static const uint32_t S21 = 5;
        static const uint32_t S22 = 9;
        static const uint32_t S23 = 14;
        static const uint32_t S24 = 20;
        static const uint32_t S31 = 4;
        static const uint32_t S32 = 11;
        static const uint32_t S33 = 16;
        static const uint32_t S34 = 23;
        static const uint32_t S41 = 6;
        static const uint32_t S42 = 10;
        static const uint32_t S43 = 15;
        static const uint32_t S44 = 21;

        /// Remainder of last data, not fitted into 64 byte block.
        uint8_t buffer[BLOCK_SIZE];

        /// Total number of bits hashed.
        uint64_t bitCount = 0;

        /// Current digest.
        uint32_t state[DIGEST_LEN / 4];

    public:

        Md5()
        {
            this->state[0] = 0x67452301;
            this->state[1] = 0xefcdab89;
            this->state[2] = 0x98badcfe;
            this->state[3] = 0x10325476;
        }

        void Update(const std::vector<uint8_t>& data)
        {
            this->Update(data.data(), data.size());
        }

        void Update(const std::string& data)
        {
            this->Update(reinterpret_cast<const uint8_t*>(data.data()), data.length());
        }

        void Update(const uint8_t* data, size_t length)
        {
            // compute number of bytes mod 64
            auto index = this->bitCount / 8 % BLOCK_SIZE;

            this->bitCount += (length << 3);

            // number of bytes we need to fill in buffer
            auto firstpart = 64 - index;

            size_t i;

            // transform as many times as possible.
            if (length >= firstpart)
            {
                // fill buffer first, transform
                std::copy(data, data + firstpart, this->buffer + index);

                this->Transform(buffer);

                // transform chunks of BLOCK_SIZE (64 bytes)
                for (i = firstpart; i + BLOCK_SIZE <= length; i += BLOCK_SIZE)
                {
                    this->Transform(&data[i]);
                }

                index = 0;
            }
            else
            {
                i = 0;
            }

            // buffer remaining input
            std::copy(data + i, data + length, this->buffer + index);
        }

        std::vector<uint8_t> Finish()
        {
            uint8_t padding[64] =
            {
                0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            };

            // Save number of bits
            uint8_t bits[8];
            this->Encode(bits, reinterpret_cast<uint32_t*>(&this->bitCount), 8);

            // pad out to 56 mod 64.
            auto index = this->bitCount / 8 % 64;
            auto padLen = (index < 56) ? (56 - index) : (120 - index);
            this->Update(padding, padLen);

            // Append length (before padding)
            this->Update(bits, 8);

            auto result = std::vector<uint8_t>(DIGEST_LEN);

            // Store state in digest
            this->Encode(result.data(), this->state, DIGEST_LEN);

            return result;
        }

    private:

        void Transform(const uint8_t block[BLOCK_SIZE])
        {
            auto a = this->state[0];
            auto b = this->state[1];
            auto c = this->state[2];
            auto d = this->state[3];

            uint32_t x[16];

            this->Decode(x, block, BLOCK_SIZE);

            this->FF (a, b, c, d, x[ 0], S11, 0xd76aa478);
            this->FF (d, a, b, c, x[ 1], S12, 0xe8c7b756);
            this->FF (c, d, a, b, x[ 2], S13, 0x242070db);
            this->FF (b, c, d, a, x[ 3], S14, 0xc1bdceee);
            this->FF (a, b, c, d, x[ 4], S11, 0xf57c0faf);
            this->FF (d, a, b, c, x[ 5], S12, 0x4787c62a);
            this->FF (c, d, a, b, x[ 6], S13, 0xa8304613);
            this->FF (b, c, d, a, x[ 7], S14, 0xfd469501);
            this->FF (a, b, c, d, x[ 8], S11, 0x698098d8);
            this->FF (d, a, b, c, x[ 9], S12, 0x8b44f7af);
            this->FF (c, d, a, b, x[10], S13, 0xffff5bb1);
            this->FF (b, c, d, a, x[11], S14, 0x895cd7be);
            this->FF (a, b, c, d, x[12], S11, 0x6b901122);
            this->FF (d, a, b, c, x[13], S12, 0xfd987193);
            this->FF (c, d, a, b, x[14], S13, 0xa679438e);
            this->FF (b, c, d, a, x[15], S14, 0x49b40821);

            this->GG (a, b, c, d, x[ 1], S21, 0xf61e2562);
            this->GG (d, a, b, c, x[ 6], S22, 0xc040b340);
            this->GG (c, d, a, b, x[11], S23, 0x265e5a51);
            this->GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa);
            this->GG (a, b, c, d, x[ 5], S21, 0xd62f105d);
            this->GG (d, a, b, c, x[10], S22,  0x2441453);
            this->GG (c, d, a, b, x[15], S23, 0xd8a1e681);
            this->GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8);
            this->GG (a, b, c, d, x[ 9], S21, 0x21e1cde6);
            this->GG (d, a, b, c, x[14], S22, 0xc33707d6);
            this->GG (c, d, a, b, x[ 3], S23, 0xf4d50d87);
            this->GG (b, c, d, a, x[ 8], S24, 0x455a14ed);
            this->GG (a, b, c, d, x[13], S21, 0xa9e3e905);
            this->GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8);
            this->GG (c, d, a, b, x[ 7], S23, 0x676f02d9);
            this->GG (b, c, d, a, x[12], S24, 0x8d2a4c8a);

            this->HH (a, b, c, d, x[ 5], S31, 0xfffa3942);
            this->HH (d, a, b, c, x[ 8], S32, 0x8771f681);
            this->HH (c, d, a, b, x[11], S33, 0x6d9d6122);
            this->HH (b, c, d, a, x[14], S34, 0xfde5380c);
            this->HH (a, b, c, d, x[ 1], S31, 0xa4beea44);
            this->HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9);
            this->HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60);
            this->HH (b, c, d, a, x[10], S34, 0xbebfbc70);
            this->HH (a, b, c, d, x[13], S31, 0x289b7ec6);
            this->HH (d, a, b, c, x[ 0], S32, 0xeaa127fa);
            this->HH (c, d, a, b, x[ 3], S33, 0xd4ef3085);
            this->HH (b, c, d, a, x[ 6], S34,  0x4881d05);
            this->HH (a, b, c, d, x[ 9], S31, 0xd9d4d039);
            this->HH (d, a, b, c, x[12], S32, 0xe6db99e5);
            this->HH (c, d, a, b, x[15], S33, 0x1fa27cf8);
            this->HH (b, c, d, a, x[ 2], S34, 0xc4ac5665);

            this->II (a, b, c, d, x[ 0], S41, 0xf4292244);
            this->II (d, a, b, c, x[ 7], S42, 0x432aff97);
            this->II (c, d, a, b, x[14], S43, 0xab9423a7);
            this->II (b, c, d, a, x[ 5], S44, 0xfc93a039);
            this->II (a, b, c, d, x[12], S41, 0x655b59c3);
            this->II (d, a, b, c, x[ 3], S42, 0x8f0ccc92);
            this->II (c, d, a, b, x[10], S43, 0xffeff47d);
            this->II (b, c, d, a, x[ 1], S44, 0x85845dd1);
            this->II (a, b, c, d, x[ 8], S41, 0x6fa87e4f);
            this->II (d, a, b, c, x[15], S42, 0xfe2ce6e0);
            this->II (c, d, a, b, x[ 6], S43, 0xa3014314);
            this->II (b, c, d, a, x[13], S44, 0x4e0811a1);
            this->II (a, b, c, d, x[ 4], S41, 0xf7537e82);
            this->II (d, a, b, c, x[11], S42, 0xbd3af235);
            this->II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb);
            this->II (b, c, d, a, x[ 9], S44, 0xeb86d391);

            this->state[0] += a;
            this->state[1] += b;
            this->state[2] += c;
            this->state[3] += d;
        }

        void Decode(uint32_t output[], const uint8_t input[], size_t len)
        {
            for (size_t i = 0, j = 0; j < len; i++, j += 4)
            {
               output[i] =
                    ((uint32_t)input[j]) |
                    (((uint32_t)input[j+1]) << 8) |
                    (((uint32_t)input[j+2]) << 16) |
                    (((uint32_t)input[j+3]) << 24);
            }
        }

        void Encode(uint8_t output[], const uint32_t input[], size_t len)
        {
            for (size_t i = 0, j = 0; j < len; i++, j += 4)
            {
                output[j] = input[i] & 0xff;
                output[j+1] = (input[i] >> 8) & 0xff;
                output[j+2] = (input[i] >> 16) & 0xff;
                output[j+3] = (input[i] >> 24) & 0xff;
            }
        }

        // low level logic operation
        uint32_t F(uint32_t x, uint32_t y, uint32_t z)
        {
            return (x & y) | (~x & z);
        }

        uint32_t G(uint32_t x, uint32_t y, uint32_t z)
        {
            return (x & z) | (y & ~z);
        }

        uint32_t H(uint32_t x, uint32_t y, uint32_t z)
        {
            return x^y^z;
        }

        uint32_t I(uint32_t x, uint32_t y, uint32_t z)
        {
            return y ^ (x | ~z);
        }

        uint32_t RotateLeft(uint32_t x, int n)
        {
            return (x << n) | (x >> (32 - n));
        }

        void FF(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
        {
            a = this->RotateLeft(a + this->F(b,c,d) + x + ac, s) + b;
        }

        void GG(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
        {
            a = this->RotateLeft(a + this->G(b,c,d) + x + ac, s) + b;
        }

        void HH(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
        {
            a = this->RotateLeft(a + this->H(b,c,d) + x + ac, s) + b;
        }

        void II(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
        {
            a = this->RotateLeft(a + this->I(b,c,d) + x + ac, s) + b;
        }
    };
} }
