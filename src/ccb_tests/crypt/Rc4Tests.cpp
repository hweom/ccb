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

#include <cxxtest/TestSuite.h>

#include <random>

#include <openssl/evp.h>
#include <openssl/rc4.h>

#include <ccb/crypt/Rc4.hpp>

namespace ccb { namespace crypt
{
    class Rc4Tests : public CxxTest::TestSuite
    {
    public:

        void TestEncodeAndDecode()
        {
            std::default_random_engine engine;

            for (size_t i = 0; i < 1; i++)
            {
                // Generate key.
                auto keyLength = 16;
                auto key = std::vector<uint8_t>(keyLength);
                for (size_t j = 0; j < keyLength; j++)
                {
                    key[j] = static_cast<uint8_t>(std::uniform_int_distribution<uint32_t>(0, 255)(engine));
                }

                // Generate input sequence.
                auto sequenceLength = std::uniform_int_distribution<uint32_t>(16, 255)(engine);
                auto in = std::vector<uint8_t>(sequenceLength);
                for (size_t j = 0; j < sequenceLength; j++)
                {
                    in[j] = static_cast<uint8_t>(std::uniform_int_distribution<uint32_t>(0, 255)(engine));
                }

                auto out1 = Rc4(key).Encrypt(in);
                auto out2 = this->EncodeByOpenSsl(key, in);

                TS_ASSERT_EQUALS(out2.size(), out1.size());
                for (size_t i = 0; i < out1.size(); i++)
                {
                    TS_ASSERT_EQUALS(out2[i], out1[i]);
                }

                auto in2 = Rc4(key).Decrypt(out1);

                TS_ASSERT_EQUALS(in.size(), in2.size());
                for (size_t i = 0; i < in2.size(); i++)
                {
                    TS_ASSERT_EQUALS(in[i], in2[i]);
                }
            }
        }

    protected:

        std::vector<uint8_t> EncodeByOpenSsl(const std::vector<uint8_t>& key, const std::vector<uint8_t>& in)
        {
            std::vector<uint8_t> result(in.size());
            EVP_CIPHER_CTX context;

            EVP_CIPHER_CTX_init(&context);

            int outLength1 = 0;
            int outLength2 = 0;

            EVP_EncryptInit(&context, EVP_rc4(), reinterpret_cast<const unsigned char*>(key.data()), nullptr);
            EVP_EncryptUpdate(&context, result.data(), &outLength1, reinterpret_cast<const unsigned char*>(in.data()), in.size());
            EVP_EncryptFinal(&context, result.data() + outLength1, &outLength2);

            EVP_CIPHER_CTX_cleanup(&context);

            return result;
        }
    };
} }
