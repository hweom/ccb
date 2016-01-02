#pragma once

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>

namespace ccb { namespace crypt {
struct NoPadding {
    uint8_t GetPadByte(size_t totalPaddingLength, size_t paddingByte) {
        throw std::runtime_error("Data length must be multiple of block size.");
    }

    size_t GetPadLength(uint8_t* data, size_t length) {
        return 0;
    }
};

struct Pkcs7 {
    uint8_t GetPadByte(size_t totalPaddingLength, size_t paddingByte) {
        return totalPaddingLength;
    }

    size_t GetPadLength(uint8_t* data, size_t length) {
        assert (length > 0);

        auto pad = data[length - 1];
        if (pad > length) {
            return 0;
        }

        for (size_t i = 0; i < pad; i++) {
            if (data[length - 1 - i] != pad) {
                return 0;
            }
        }

        return pad;
    }
};
} }
