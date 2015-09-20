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
#include <vector>

namespace ccb { namespace crypt
{
    /// An implementation of RC4 cipher.
    class Rc4
    {
    private:

        /// Cipher state.
        uint8_t state[256];

        size_t i = 0;

        size_t j = 0;

    public:

        Rc4(const std::vector<uint8_t>& key)
        {
            for (size_t k = 0; k < 256; k++)
            {
                this->state[k] = k;
            }

            size_t j = 0;
            for (size_t k = 0; k < 256; k++)
            {
               j = (j + this->state[k] + key[k % key.size()]) % 256;

               auto t = this->state[k];
               this->state[k] = this->state[j];
               this->state[j] = t;
            }
        }

    public:

        std::vector<uint8_t> Encrypt(const std::vector<uint8_t>& in)
        {
            std::vector<uint8_t> result(in.size());

            for (size_t k = 0; k < in.size(); k++)
            {
                this->i = (this->i + 1) % 256;
                this->j = (this->j + this->state[this->i]) % 256;

                auto t = this->state[this->i];
                this->state[this->i] = this->state[this->j];
                this->state[this->j] = t;

                auto b = this->state[(this->state[this->i] + this->state[this->j]) % 256];

                result[k] = in[k] ^ b;
            }

            return result;
        }

        std::vector<uint8_t> Decrypt(const std::vector<uint8_t>& in)
        {
            return this->Encrypt(in);
        }
    };
} }
