#pragma once

#include <cstdint>
#include <unordered_map>

#include <ccb/charset/Encoding.hpp>
#include <ccb/charset/Cp866Table.hpp>
#include <ccb/charset/Cp1251Table.hpp>
#include <ccb/charset/Koi8Table.hpp>

#define CCB_CHARSET_DECLARE_PAGE(enc, page)     \
template<>                                      \
struct CharsetPageSelector<enc>                 \
{                                               \
    static const bool IS_CHARSET_PAGE = true;   \
                                                \
    using Page = CharsetPage<page>;             \
};

namespace ccb { namespace charset
{
    template<typename Table>
    class CharsetPage
    {
    public:

        uint8_t FromUtf(uint32_t codepoint) const
        {
            const auto& map = GetUnicodeMap();

            auto pos = map.find(codepoint);

            return (pos != map.end()) ? pos->second : 0;
        }

        uint32_t ToUtf(uint8_t byte) const
        {
            if (byte < 0x80)
            {
                return byte;
            }
            else
            {
                auto codePoint = Table::GetHiTable()[static_cast<unsigned>(byte) - 0x80];

                if (codePoint == 0)
                {
                    return byte;
                }
            }
        }

    private:

        static std::unordered_map<uint32_t, uint8_t>& GetUnicodeMap()
        {
            static std::unordered_map<uint32_t, uint8_t> map = CreateUnicodeMap();

            return map;
        }

        static std::unordered_map<uint32_t, uint8_t> CreateUnicodeMap()
        {
            std::unordered_map<uint32_t, uint8_t> map;

            const auto& table = Table::GetHiTable();

            for (size_t i = 0; i < 0x80; i++)
            {
                auto codePoint = table[i];
                if (codePoint == 0)
                {
                    continue;
                }

                map[codePoint] = i + 0x80;
            }

            return map;
        }
    };

    template<Encoding enc>
    struct CharsetPageSelector
    {
        static const bool IS_CHARSET_PAGE = false;
    };

    CCB_CHARSET_DECLARE_PAGE(Encoding::CP_866, Cp866Table)
    CCB_CHARSET_DECLARE_PAGE(Encoding::CP_1251, Cp1251Table)
    CCB_CHARSET_DECLARE_PAGE(Encoding::KOI8_R, Koi8Table)
} }
