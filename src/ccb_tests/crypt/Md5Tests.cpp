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

#include <openssl/md5.h>

#include <ccb/crypt/Md5.hpp>

namespace ccb { namespace crypt
{
    class Md5Tests : public CxxTest::TestSuite
    {
    public:

        void TestHash()
        {
            std::default_random_engine engine;

            for (size_t i = 0; i < 10; i++)
            {
                auto msgLen = std::uniform_int_distribution<uint32_t>(100, 2000)(engine);

                auto msg = std::vector<uint8_t>(msgLen);
                for (size_t j = 0; j < msgLen; j++)
                {
                    msg[j] = std::uniform_int_distribution<uint32_t>(0, 255)(engine);
                }

                auto digest1 = this->HashByMd5(msg);
                auto digest2 = this->HashByOpenSsl(msg);

                TS_ASSERT_EQUALS(digest2.size(), digest1.size());
                for (size_t j = 0; j < digest1.size(); j++)
                {
                    TS_ASSERT_EQUALS(digest2[j], digest1[j]);
                }
            }
        }

    private:

        std::vector<uint8_t> HashByMd5(const std::vector<uint8_t>& data)
        {
            Md5 md5;
            md5.Update(data);

            return md5.Finish();
        }

        std::vector<uint8_t> HashByOpenSsl(const std::vector<uint8_t>& data)
        {
            MD5_CTX md5;
            std::vector<uint8_t> result(MD5_DIGEST_LENGTH);

            MD5_Init(&md5);
            MD5_Update(&md5, reinterpret_cast<const unsigned char*>(data.data()), data.size());
            MD5_Final(result.data(), &md5);

            return result;
        }
    };
} }
