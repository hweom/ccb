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

#include <ccb/crypt/Aes.hpp>

namespace ccb { namespace crypt {
class AesTests : public CxxTest::TestSuite {
public:

    void TestCanEncryptAesEcb() {
        auto plaintext = this->FromHex(
            "6bc1bee22e409f96e93d7e117393172a"
            "ae2d8a571e03ac9c9eb76fac45af8e51"
            "30c81c46a35ce411e5fbc1191a0a52ef"
            "f69f2445df4f9b17ad2b417be66c3710");

        auto key = this->FromHex(
            "2b7e151628aed2a6abf7158809cf4f3c");

        auto ciphertext = this->FromHex(
            "3ad77bb40d7a3660a89ecaf32466ef97"
            "f5d3d58503b9699de785895a96fdbaaf"
            "43b1cd7f598ece23881b00e3ed030688"
            "7b0c785e27e8ad3f8223207104725dd4");

        auto output = std::vector<uint8_t>();

        auto aes = Aes<>(key.data());

        aes.EncryptEcb<NoPadding>(plaintext.begin(), plaintext.end(), std::back_inserter(output));

        TS_ASSERT_EQUALS(ciphertext.size(), output.size());
        for (size_t i = 0; i < ciphertext.size(); i++) {
            TS_ASSERT_EQUALS(ciphertext[i], output[i]);
        }
    }

    void TestCanDecryptAesEcb() {
        auto plaintext = this->FromHex(
            "6bc1bee22e409f96e93d7e117393172a"
            "ae2d8a571e03ac9c9eb76fac45af8e51"
            "30c81c46a35ce411e5fbc1191a0a52ef"
            "f69f2445df4f9b17ad2b417be66c3710");

        auto key = this->FromHex(
            "2b7e151628aed2a6abf7158809cf4f3c");

        auto ciphertext = this->FromHex(
            "3ad77bb40d7a3660a89ecaf32466ef97"
            "f5d3d58503b9699de785895a96fdbaaf"
            "43b1cd7f598ece23881b00e3ed030688"
            "7b0c785e27e8ad3f8223207104725dd4");

        auto output = std::vector<uint8_t>();

        auto aes = Aes<>(key.data());

        aes.DecryptEcb<NoPadding>(ciphertext.begin(), ciphertext.end(), std::back_inserter(output));

        TS_ASSERT_EQUALS(plaintext.size(), output.size());
        for (size_t i = 0; i < plaintext.size(); i++) {
            TS_ASSERT_EQUALS(plaintext[i], output[i]);
        }
    }

    void TestCanEncryptAesCbcNoPadding() {
        auto iv = this->FromHex(
            "538efbaf48c92f8a46f46c44f74ac5b0");

        auto plaintext = this->FromHex(
            "6bc1bee22e409f96e93d7e117393172a"
            "ae2d8a571e03ac9c9eb76fac45af8e51"
            "30c81c46a35ce411e5fbc1191a0a52ef"
            "f69f2445df4f9b17ad2b417be66c3710");

        auto key = this->FromHex(
            "2b7e151628aed2a6abf7158809cf4f3c");

        auto ciphertext = this->FromHex(
            "89faf380a384cfea57767ec09f39c33d"
            "b6a97d78d9de826124ea343efeb9dccf"
            "a952d4a90e20b0afbe17197499ab2029"
            "9b901776e803e3b17155545d93d639d9");

        auto output = std::vector<uint8_t>();

        auto aes = Aes<>(key.data());

        aes.EncryptCbc<NoPadding>(plaintext.begin(), plaintext.end(), std::back_inserter(output), iv.begin());

        TS_ASSERT_EQUALS(ciphertext.size(), output.size());
        for (size_t i = 0; i < ciphertext.size(); i++) {
            TS_ASSERT_EQUALS(ciphertext[i], output[i]);
        }
    }

    void TestCanDecryptAesCbcNoPadding() {
        auto iv = this->FromHex(
            "538efbaf48c92f8a46f46c44f74ac5b0");

        auto plaintext = this->FromHex(
            "6bc1bee22e409f96e93d7e117393172a"
            "ae2d8a571e03ac9c9eb76fac45af8e51"
            "30c81c46a35ce411e5fbc1191a0a52ef"
            "f69f2445df4f9b17ad2b417be66c3710");

        auto key = this->FromHex(
            "2b7e151628aed2a6abf7158809cf4f3c");

        auto ciphertext = this->FromHex(
            "89faf380a384cfea57767ec09f39c33d"
            "b6a97d78d9de826124ea343efeb9dccf"
            "a952d4a90e20b0afbe17197499ab2029"
            "9b901776e803e3b17155545d93d639d9");

        auto output = std::vector<uint8_t>();

        auto aes = Aes<>(key.data());

        aes.DecryptCbc<NoPadding>(ciphertext.begin(), ciphertext.end(), std::back_inserter(output), iv.begin());

        TS_ASSERT_EQUALS(plaintext.size(), output.size());
        for (size_t i = 0; i < plaintext.size(); i++) {
            TS_ASSERT_EQUALS(plaintext[i], output[i]);
        }
    }

    void TestCanEncryptAndDecryptAesCbcPkcs7Padding() {
        auto iv = this->FromHex(
            "538efbaf48c92f8a46f46c44f74ac5b0");

        auto plaintext = this->FromHex(
            "6bc1bee22e409f96e93d7e117393172a"
            "ae2d8a571e03ac9c9eb76fac45af8e51"
            "30c81c46a35ce411e5fbc1191a0a52ef"
            "f69f2445df4f9b17ad2b417be6");

        auto key = this->FromHex(
            "2b7e151628aed2a6abf7158809cf4f3c");

        auto ciphertext = std::vector<uint8_t>();

        auto aes = Aes<>(key.data());

        aes.EncryptCbc<Pkcs7>(plaintext.begin(), plaintext.end(), std::back_inserter(ciphertext), iv.begin());

        auto plaintext2 = std::vector<uint8_t>();

        aes.DecryptCbc<Pkcs7>(ciphertext.begin(), ciphertext.end(), std::back_inserter(plaintext2), iv.begin());

        TS_ASSERT_EQUALS(plaintext.size(), plaintext2.size());
        for (size_t i = 0; i < plaintext.size(); i++) {
            TS_ASSERT_EQUALS(plaintext[i], plaintext2[i]);
        }
    }

    void TestCanEncryptAndDecryptAesCbcPkcs7FullBlockPadding() {
        auto iv = this->FromHex(
            "538efbaf48c92f8a46f46c44f74ac5b0");

        auto plaintext = this->FromHex(
            "6bc1bee22e409f96e93d7e117393172a"
            "ae2d8a571e03ac9c9eb76fac45af8e51"
            "30c81c46a35ce411e5fbc1191a0a52ef"
            "f69f2445df4f9b17ad2b417be66c3710");

        auto key = this->FromHex(
            "2b7e151628aed2a6abf7158809cf4f3c");

        auto ciphertext = std::vector<uint8_t>();

        auto aes = Aes<>(key.data());

        aes.EncryptCbc<Pkcs7>(plaintext.begin(), plaintext.end(), std::back_inserter(ciphertext), iv.begin());

        TS_ASSERT_EQUALS(80, ciphertext.size());

        auto plaintext2 = std::vector<uint8_t>();

        aes.DecryptCbc<Pkcs7>(ciphertext.begin(), ciphertext.end(), std::back_inserter(plaintext2), iv.begin());

        TS_ASSERT_EQUALS(plaintext.size(), plaintext2.size());
        for (size_t i = 0; i < plaintext.size(); i++) {
            TS_ASSERT_EQUALS(plaintext[i], plaintext2[i]);
        }
    }

    void TestCanDecryptAesCbcInplace() {
        auto iv = this->FromHex(
            "538efbaf48c92f8a46f46c44f74ac5b0");

        auto plaintext = this->FromHex(
            "6bc1bee22e409f96e93d7e117393172a"
            "ae2d8a571e03ac9c9eb76fac45af8e51"
            "30c81c46a35ce411e5fbc1191a0a52ef"
            "f69f2445df4f9b17ad2b417be66c3710");

        auto key = this->FromHex(
            "2b7e151628aed2a6abf7158809cf4f3c");

        auto ciphertext = std::vector<uint8_t>();

        auto aes = Aes<>(key.data());

        aes.EncryptCbc<Pkcs7>(plaintext.begin(), plaintext.end(), std::back_inserter(ciphertext), iv.begin());

        auto ciphertextWithIv = std::vector<uint8_t>();
        std::copy(iv.begin(), iv.end(), std::back_inserter(ciphertextWithIv));
        std::copy(ciphertext.begin(), ciphertext.end(), std::back_inserter(ciphertextWithIv));

        auto plaintext2 = std::vector<uint8_t>();

        auto pos = aes.DecryptCbc<Pkcs7>(ciphertextWithIv.begin() + 16, ciphertextWithIv.end(), ciphertextWithIv.begin(), ciphertextWithIv.begin());

        TS_ASSERT_EQUALS(64, std::distance(ciphertextWithIv.begin(), pos));

        for (size_t i = 0; i < plaintext.size(); i++) {
            TS_ASSERT_EQUALS(plaintext[i], ciphertextWithIv[i]);
        }
    }

private:

    std::vector<uint8_t> FromHex(const std::string& hex) {
        std::vector<uint8_t> result(hex.size() / 2);

        auto pos = result.begin();
        for (size_t i = 0; i < hex.size(); i += 2) {
            auto c0 = tolower(hex[i]);
            auto c1 = tolower(hex[i + 1]);

            auto byte =
                (static_cast<uint8_t>((c0 >= 'a') ? (0xa + (c0 - 'a')) : (c0 - '0')) << 4) |
                (static_cast<uint8_t>((c1 >= 'a') ? (0xa + (c1 - 'a')) : (c1 - '0')));

            *(pos++) = byte;
        }

        return result;
    }
};
} }
