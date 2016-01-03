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
#include <stdexcept>
#include <string>
#include <vector>

#include <ccb/crypt/Padding.hpp>

namespace ccb { namespace crypt {
/// AES encryption implementation.
template<size_t KeySize=128>
class Aes {
private:

    static constexpr size_t GetNr() {
        return (KeySize == 128)
            ? 10
            : ((KeySize == 192) ? 12 : 14);
    }

private:

    static const size_t BLOCK_SIZE = 16;

    static const size_t Nb = 4;

    static const size_t Nk = KeySize / 32;

    static const size_t Nr = GetNr();

    uint8_t roundKeys[Nr + 1][4 * Nb];

    mutable uint8_t state[4][Nb];

public:

    Aes(const uint8_t* key) {
        this->CreateKeys(key);
    }

    Aes(const std::string& key)
        : Aes<KeySize>(reinterpret_cast<const uint8_t*>(key.data())) {
    }

public:

    /// Encrypt data in ECB mode.
    /// Input data must be multiple of BLOCK_SIZE in length.
    template<typename Padding, typename InIter, typename OutIter>
    OutIter EncryptEcb(InIter inBegin, InIter inEnd, OutIter outBegin) const {
        uint8_t buffer[BLOCK_SIZE];
        bool paddingDone = false;

        while (inBegin != inEnd) {
            auto pair = this->BlockToState<Padding>(inBegin, inEnd);
            inBegin = pair.first;
            paddingDone = (pair.second < BLOCK_SIZE);

            this->EncryptBlock();

            outBegin = this->StateToBlock(outBegin);
        }

        // Add last block with padding.
        if (!paddingDone && !std::is_same<Padding, NoPadding>::value) {
            this->BlockToState<Padding>(inBegin, inEnd);
            this->EncryptBlock();
            outBegin = this->StateToBlock(outBegin);
        }

        return outBegin;
    }

    /// Decrypt data in ECB mode.
    /// Input data must be multiple of BLOCK_SIZE in length.
    template<typename Padding, typename InIter, typename OutIter>
    OutIter DecryptEcb(InIter inBegin, InIter inEnd, OutIter outBegin) const {
        uint8_t buffer[BLOCK_SIZE];

        while (inBegin != inEnd) {
            inBegin = this->BlockToState<NoPadding>(inBegin, inEnd).first;
            this->DecryptBlock();
            this->StateToBlock(buffer);

            // Check last block for padding.
            if (inBegin == inEnd) {
                auto padding = Padding().GetPadLength(buffer, BLOCK_SIZE);
                outBegin = std::copy(buffer, buffer + (BLOCK_SIZE - padding), outBegin);
            }
            else {
                outBegin = std::copy(buffer, buffer + BLOCK_SIZE, outBegin);
            }
        }

        return outBegin;
    }

    /// Encrypt data in CBC mode.
    template<typename Padding, typename InIter, typename OutIter, typename IvIter>
    OutIter EncryptCbc(InIter inBegin, InIter inEnd, OutIter outBegin, IvIter ivBegin) const {
        uint8_t buffer[BLOCK_SIZE];
        bool paddingDone = false;

        std::copy(ivBegin, ivBegin + BLOCK_SIZE, buffer);

        while (inBegin != inEnd) {
            auto pair = this->BlockToState<Padding>(inBegin, inEnd);
            inBegin = pair.first;
            paddingDone = (pair.second < BLOCK_SIZE);

            this->Xor(buffer);
            this->EncryptBlock();

            this->StateToBlock(buffer);
            outBegin = std::copy(buffer, buffer + BLOCK_SIZE, outBegin);
        }

        // Add last block with padding.
        if (!paddingDone && !std::is_same<Padding, NoPadding>::value) {
            this->BlockToState<Padding>(inBegin, inEnd);
            this->Xor(buffer);
            this->EncryptBlock();
            outBegin = this->StateToBlock(outBegin);
        }

        return outBegin;
    }

    /// Decrypt data in CBC mode.
    template<typename Padding, typename InIter, typename OutIter, typename IvIter>
    OutIter DecryptCbc(InIter inBegin, InIter inEnd, OutIter outBegin, IvIter ivBegin) const {
        std::vector<uint8_t> buffer0(BLOCK_SIZE);
        std::vector<uint8_t> buffer1(BLOCK_SIZE);

        std::copy(ivBegin, ivBegin + BLOCK_SIZE, buffer1.begin());

        while (inBegin != inEnd) {
            for (size_t i = 0; i < BLOCK_SIZE; i++) {
                if (inBegin == inEnd) {
                    throw std::runtime_error("Ciphertext length not multiple of 16");
                }

                buffer0[i] = *(inBegin++);
            }

            this->BlockToState<NoPadding>(buffer0.begin(), buffer0.end());
            this->DecryptBlock();
            this->Xor(buffer1.begin());
            this->StateToBlock(buffer1.begin());

            // This is the last block - check for padding.
            if (inBegin == inEnd) {
                auto padding = Padding().GetPadLength(buffer1.data(), BLOCK_SIZE);
                outBegin = std::copy(buffer1.begin(), buffer1.begin() + (BLOCK_SIZE - padding), outBegin);
            }
            else {
                outBegin = std::copy(buffer1.begin(), buffer1.end(), outBegin);
            }

            std::swap(buffer0, buffer1);
        }

        return outBegin;
    }

private:

    void EncryptBlock() const {
        this->AddRoundKey(0);

        for (size_t i = 1; i < Nr; i++) {
            this->SubBytes();
            this->ShiftRows();
            this->MixColumns();
            this->AddRoundKey(i);
        }

        this->SubBytes();
        this->ShiftRows();
        this->AddRoundKey(Nr);
    }

    void DecryptBlock() const {
        this->AddRoundKey(Nr);

        for(size_t i = Nr - 1; i > 0; i--) {
            this->ShiftRowsInv();
            this->SubBytesInv();
            this->AddRoundKey(i);
            this->MixColumnsInv();
        }

        this->ShiftRowsInv();
        this->SubBytesInv();
        this->AddRoundKey(0);
    }

    template<typename Iter>
    void Xor(Iter xorData) const {
        for (size_t c = 0; c < Nb; c++) {
            for (size_t r = 0; r < 4; r++) {
                this->state[c][r] ^= *(xorData++);
            }
        }
    }

    void CreateKeys(const uint8_t* keyData) {
        auto keyWords = reinterpret_cast<const uint32_t*>(keyData);
        auto roundKeyWords = reinterpret_cast<uint32_t*>(this->roundKeys);

        for (size_t i = 0; i < Nk; i++) {
            roundKeyWords[i] = keyWords[i];
        }

        for (size_t i = Nk; i < Nb * (Nr + 1); i++) {
            auto word = roundKeyWords[i - 1];
            if (i % Nk == 0) {
                word = this->SubWord(RotWord(word)) ^ this->GetRcon(i / Nk);
            }
            else if ((Nk > 6) && (i % Nk == 4)) {
                word = this->SubWord(word);
            }

            roundKeyWords[i] = roundKeyWords[i - Nk] ^ word;
        }
    }

    template<typename Padding, typename InIter>
    std::pair<InIter, size_t> BlockToState(InIter begin, InIter end) const {
        Padding padding;
        uint8_t paddingLength = 0;
        size_t length = 0;
        for (size_t c = 0; c < Nb; c++) {
            for (size_t r = 0; r < 4; r++, length++) {
                // Set padding byte
                if ((begin == end) && (paddingLength == 0)) {
                    paddingLength = BLOCK_SIZE - length;
                }

                this->state[c][r] = (begin != end) ? (*begin) : padding.GetPadByte(paddingLength, length);

                if (begin != end) {
                    begin++;
                }
            }
        }

        return std::make_pair(begin, BLOCK_SIZE - paddingLength);
    }

    template<typename OutIter>
    OutIter StateToBlock(OutIter out) const {
        size_t i = 0;
        for (size_t c = 0; c < Nb; c++, i++) {
            for (size_t r = 0; r < 4; r++) {
                *(out++) = this->state[c][r];
            }
        }

        return out;
    }

    void SubBytes() const {
        for (size_t r = 0; r < 4; r++) {
            for (size_t c = 0; c < Nb; c++) {
                this->state[c][r] = this->GetSBox(this->state[c][r]);
            }
        }
    }

    void SubBytesInv() const {
        for (size_t r = 0; r < 4; r++) {
            for (size_t c = 0; c < Nb; c++) {
                this->state[c][r] = this->GetSBoxInv(this->state[c][r]);
            }
        }
    }

    void ShiftRows() const {
        for (size_t r = 0; r < 4; r++) {
            for (size_t i = 0; i < r; i++) {
                auto b = this->state[0][r];
                for (size_t c = 0; c < Nb - 1; c++) {
                    this->state[c][r] = this->state[(c + 1) % Nb][r];
                }

                this->state[Nb - 1][r] = b;
            }
        }
    }

    void ShiftRowsInv() const {
        for (size_t r = 0; r < 4; r++) {
            for (size_t i = 0; i < r; i++) {
                auto b = this->state[Nb - 1][r];
                for (size_t c = Nb - 1; c > 0; c--) {
                    this->state[c][r] = this->state[(c + Nb - 1) % Nb][r];
                }

                this->state[0][r] = b;
            }
        }
    }

    void MixColumns() const {
        uint8_t i;
        uint8_t Tmp,Tm,t;
        for(i = 0; i < 4; i++) {
            t = this->state[i][0];
            Tmp = this->state[i][0] ^ this->state[i][1] ^ this->state[i][2] ^ this->state[i][3] ;
            Tm  = this->state[i][0] ^ this->state[i][1];
            Tm = xtime(Tm);
            this->state[i][0] ^= Tm ^ Tmp;

            Tm  = this->state[i][1] ^ this->state[i][2];
            Tm = xtime(Tm);
            this->state[i][1] ^= Tm ^ Tmp;

            Tm  = this->state[i][2] ^ this->state[i][3];
            Tm = xtime(Tm);
            this->state[i][2] ^= Tm ^ Tmp;

            Tm  = this->state[i][3] ^ t;
            Tm = xtime(Tm);
            this->state[i][3] ^= Tm ^ Tmp;
        }
    }

    void MixColumnsInv() const {
        int i;
        uint8_t a,b,c,d;
        for(i=0;i<4;++i) {
            a = this->state[i][0];
            b = this->state[i][1];
            c = this->state[i][2];
            d = this->state[i][3];

            this->state[i][0] = this->Multiply(a, 0x0e) ^ this->Multiply(b, 0x0b) ^ this->Multiply(c, 0x0d) ^ this->Multiply(d, 0x09);
            this->state[i][1] = this->Multiply(a, 0x09) ^ this->Multiply(b, 0x0e) ^ this->Multiply(c, 0x0b) ^ this->Multiply(d, 0x0d);
            this->state[i][2] = this->Multiply(a, 0x0d) ^ this->Multiply(b, 0x09) ^ this->Multiply(c, 0x0e) ^ this->Multiply(d, 0x0b);
            this->state[i][3] = this->Multiply(a, 0x0b) ^ this->Multiply(b, 0x0d) ^ this->Multiply(c, 0x09) ^ this->Multiply(d, 0x0e);
        }
    }

    uint8_t xtime(uint8_t x) const {
        return ((x<<1) ^ (((x>>7) & 1) * 0x1b));
    }

    uint8_t Multiply(uint8_t x, uint8_t y) const {
        return (((y & 1) * x) ^
           ((y>>1 & 1) * xtime(x)) ^
           ((y>>2 & 1) * xtime(xtime(x))) ^
           ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^
           ((y>>4 & 1) * xtime(xtime(xtime(xtime(x))))));
    }

    void AddRoundKey(size_t keyNumber) const {
        for (size_t r = 0; r < 4; r++) {
            for (size_t c = 0; c < Nb; c++) {
                this->state[r][c] ^= this->roundKeys[keyNumber][r * Nb + c];
            }
        }
    }

    uint32_t SubWord(uint32_t word) const {
        return
            (static_cast<uint32_t>(this->GetSBox((word >> 24) & 0xff)) << 24) |
            (static_cast<uint32_t>(this->GetSBox((word >> 16) & 0xff)) << 16) |
            (static_cast<uint32_t>(this->GetSBox((word >>  8) & 0xff)) <<  8) |
            (static_cast<uint32_t>(this->GetSBox((word      ) & 0xff))      );
    }

    uint32_t RotWord(uint32_t word) const {
        return (word >> 8) | (word << 24);
    }

    static uint8_t GetRcon(size_t i) {
        static const uint8_t rcon[] = {
            0x8d,
            0x01,
            0x02,
            0x04,
            0x08,
            0x10,
            0x20,
            0x40,
            0x80,
            0x1b,
            0x36,
        };

        return rcon[i];
    }

    static uint8_t GetSBox(size_t i) {
        static const uint8_t sbox[] = {
            0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
            0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
            0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
            0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
            0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
            0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
            0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
            0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
            0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
            0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
            0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
            0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
            0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
            0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
            0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
            0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
        };

        return sbox[i];
    }

    static uint8_t GetSBoxInv(size_t i) {
        static const uint8_t rsbox[] = {
            0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
            0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
            0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
            0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
            0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
            0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
            0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
            0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
            0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
            0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
            0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
            0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
            0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
            0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
            0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
            0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
        };

        return rsbox[i];
    }
};
} }
