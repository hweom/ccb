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
#include <cstdint>
#include <cstdlib>
#include <string>
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
            : Rc4(key.data(), key.size())
        {
        }

        Rc4(const std::string& key)
            : Rc4(reinterpret_cast<const uint8_t*>(key.data()), key.length())
        {
        }

        Rc4(const uint8_t* key, size_t keyLength)
        {
            for (size_t k = 0; k < 256; k++)
            {
                this->state[k] = k;
            }

            size_t j = 0;
            for (size_t k = 0; k < 256; k++)
            {
               j = (j + this->state[k] + key[k % keyLength]) % 256;

               auto t = this->state[k];
               this->state[k] = this->state[j];
               this->state[j] = t;
            }
        }

    public:

        std::vector<uint8_t> Encrypt(const std::vector<uint8_t>& in)
        {
            std::vector<uint8_t> result(in.size());

            this->EncryptData(in.data(), in.size(), result.data());

            return result;
        }

        void Encrypt(const uint8_t* in, size_t length, uint8_t* out)
        {
            this->EncryptData(in, length, out);
        }

        std::string Encrypt(const std::string& in)
        {
            std::vector<uint8_t> result(in.size());

            this->EncryptData(reinterpret_cast<const uint8_t*>(in.data()), in.size(), result.data());

            return std::string(result.begin(), result.end());
        }

        std::vector<uint8_t> Decrypt(const std::vector<uint8_t>& in)
        {
            return this->Encrypt(in);
        }

        void Decrypt(const uint8_t* in, size_t length, uint8_t* out)
        {
            this->Encrypt(in, length, out);
        }

        std::string Decrypt(const std::string& in)
        {
            return this->Encrypt(in);
        }

    private:

        void EncryptData(const uint8_t* data, size_t length, uint8_t* out)
        {
            for (size_t k = 0; k < length; k++)
            {
                this->i = (this->i + 1) % 256;
                this->j = (this->j + this->state[this->i]) % 256;

                auto t = this->state[this->i];
                this->state[this->i] = this->state[this->j];
                this->state[this->j] = t;

                auto b = this->state[(this->state[this->i] + this->state[this->j]) % 256];

                out[k] = data[k] ^ b;
            }
        }
    };
} }
