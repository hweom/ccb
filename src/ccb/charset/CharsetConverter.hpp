#pragma once

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iterator>

#include <ccb/binary/ByteIterator.hpp>
#include <ccb/charset/Encoding.hpp>

namespace ccb { namespace charset
{
    namespace details
    {
        template<Encoding Enc, typename Enable = void>
        struct EncodingTraits
        {
            template<typename Iter>
            using ByteIterator = binary::ByteIterator<binary::BigEndian, 1, Iter>;

            template<typename Iter>
            Iter From (uint32_t& codePoint, Iter begin, Iter end)
            {
                codePoint = static_cast<uint32_t>(*begin);

                return ++begin;
            }

            template<typename Iter>
            Iter To(uint32_t codePoint, Iter pos)
            {
                *pos = codePoint;

                return ++pos;
            }
        };

        template<>
        struct EncodingTraits<Encoding::UTF8>
        {
            template<typename Iter>
            using ByteIterator = binary::ByteIterator<binary::BigEndian, 1, Iter>;

            template<typename Iter>
            Iter From (uint32_t& codePoint, Iter begin, Iter end)
            {
                while (begin != end)
                {
                    codePoint = 0;
                    auto b0 = static_cast<uint8_t>(*begin++);

                    if ((b0 & 0x80) == 0)
                    {
                        codePoint = b0;
                        return begin;
                    }

                    size_t bytes = 0;
                    while ((bytes < 8) && ((b0 & (0x80 >> bytes)) != 0))
                    {
                        b0 &= ~(0x80 >> bytes);
                        bytes++;
                    }

                    // Erroneous symbol - skip it.
                    if ((bytes < 2) || (bytes > 4))
                    {
                        continue;
                    }

                    codePoint = b0;

                    bool error = false;
                    for (size_t i = 1; i < bytes; i++)
                    {
                        auto b = static_cast<uint8_t>(*begin);

                        // Erroneous symbol - skip it.
                        if ((b & 0xc0) != 0x80)
                        {
                            error = true;
                            break;
                        }

                        codePoint = (codePoint << 6) | (b & 0x3f);
                        begin++;
                    }

                    if (error)
                    {
                        continue;
                    }

                    return begin;
                }

                codePoint = 0;
                return begin;
            }

            template<typename Iter>
            Iter To(uint32_t codePoint, Iter pos)
            {
                if (codePoint < 0x80)
                {
                    auto byte0 = static_cast<uint8_t>(codePoint);

                    *(pos++) = byte0;
                }
                else if (codePoint < 0x800)
                {
                    auto byte0 = static_cast<uint8_t>(((codePoint >> 6) & 0x1f) | 0xc0);
                    auto byte1 = static_cast<uint8_t>((codePoint & 0x3f) | 0x80);

                    *(pos++) = byte0;
                    *(pos++) = byte1;
                }
                else if (codePoint < 0x10000)
                {
                    auto byte0 = static_cast<uint8_t>(((codePoint >> 12) & 0x0f) | 0xe0);
                    auto byte1 = static_cast<uint8_t>(((codePoint >> 6) & 0x3f) | 0x80);
                    auto byte2 = static_cast<uint8_t>((codePoint & 0x3f) | 0x80);

                    *(pos++) = byte0;
                    *(pos++) = byte1;
                    *(pos++) = byte2;
                }
                else
                {
                    auto byte0 = static_cast<uint8_t>(((codePoint >> 18) & 0x07) | 0xf0);
                    auto byte1 = static_cast<uint8_t>(((codePoint >> 12) & 0x3f) | 0x80);
                    auto byte2 = static_cast<uint8_t>(((codePoint >> 6) & 0x3f) | 0x80);
                    auto byte3 = static_cast<uint8_t>((codePoint & 0x3f) | 0x80);

                    *(pos++) = byte0;
                    *(pos++) = byte1;
                    *(pos++) = byte2;
                    *(pos++) = byte3;
                }

                return pos;
            }
        };

        template<Encoding Enc>
        struct UtfEncodingTraits
        {
        };

        template<>
        struct UtfEncodingTraits<Encoding::UTF16BE>
        {
            template<typename Iter>
            using ByteIterator = binary::ByteIterator<binary::BigEndian, 2, Iter>;
        };

        template<>
        struct UtfEncodingTraits<Encoding::UTF16LE>
        {
            template<typename Iter>
            using ByteIterator = binary::ByteIterator<binary::LittleEndian, 2, Iter>;
        };

        template<>
        struct UtfEncodingTraits<Encoding::UTF32BE>
        {
            template<typename Iter>
            using ByteIterator = binary::ByteIterator<binary::BigEndian, 4, Iter>;
        };

        template<>
        struct UtfEncodingTraits<Encoding::UTF32LE>
        {
            template<typename Iter>
            using ByteIterator = binary::ByteIterator<binary::LittleEndian, 4, Iter>;
        };

        template<Encoding Enc>
        struct EncodingTraits<Enc, typename std::enable_if<(Enc == Encoding::UTF16BE) || (Enc == Encoding::UTF16LE), void>::type>
        {
            template<typename Iter>
            using ByteIterator = typename UtfEncodingTraits<Enc>::template ByteIterator<Iter>;

            template<typename Iter>
            Iter From (uint32_t& codePoint, Iter begin, Iter end)
            {
                auto u0 = static_cast<uint16_t>(*begin++);

                if ((u0 < 0xd800) || (u0 >= 0xdc00))
                {
                    codePoint = u0;
                    return begin;
                }

                auto u1 = static_cast<uint16_t>(*begin++);

                codePoint = (static_cast<uint32_t>(u0 - 0xd800) << 16) | (u1 - 0xdc00);

                return begin;
            }

            template<typename Iter>
            Iter To(uint32_t codePoint, Iter pos)
            {
                if (codePoint < 0x10000)
                {
                    *(pos++) = static_cast<typename std::iterator_traits<Iter>::value_type>(codePoint);
                }
                else
                {
                    codePoint -= 0x10000;
                    *(pos++) = static_cast<typename std::iterator_traits<Iter>::value_type>(0xd800 + (codePoint >> 10));
                    *(pos++) = static_cast<typename std::iterator_traits<Iter>::value_type>(0xdc00 + (codePoint & 0x3ff));
                }
            }
        };

        template<Encoding Enc>
        struct EncodingTraits<Enc, typename std::enable_if<(Enc == Encoding::UTF32BE) || (Enc == Encoding::UTF32LE), void>::type>
        {
            template<typename Iter>
            using ByteIterator = typename UtfEncodingTraits<Enc>::template ByteIterator<Iter>;

            template<typename Iter>
            Iter From (uint32_t& codePoint, Iter begin, Iter end)
            {
                codePoint = static_cast<uint32_t>(*begin);

                return ++begin;
            }

            template<typename Iter>
            Iter To(uint32_t codePoint, Iter pos)
            {
                *pos = codePoint;

                return ++pos;
            }
        };
    }

    template<Encoding To, Encoding From>
    class CharsetConverter
    {
    public:

        template<typename Iter1, typename Iter2>
        void ConvertBytes(Iter1 beg, Iter1 end, Iter2 dest)
        {
            if (From == Encoding::Unknown)
            {
                this->ConvertUtfBytes(beg, end, dest);
            }
            else
            {
                this->ConvertBytesToUnits<Iter1, Iter2>(beg, end, dest);
            }
        }

        template<typename Iter1, typename Iter2>
        void Convert(Iter1 beg, Iter1 end, Iter2 dest)
        {
            this->ConvertUnitsToUnits<Iter1, Iter2>(beg, end, dest);
        }

    private:

        template<typename Iter1, typename Iter2>
        void ConvertUtfBytes(Iter1 beg, Iter1 end, Iter2 dest)
        {
            auto bom32 = *binary::ByteIterator<binary::BigEndian, 4, Iter1>(beg, end);

            if ((bom32 & 0xffffff00) == 0xefbbbf00)
            {
                CharsetConverter<To, Encoding::UTF8>().ConvertBytes(++(++(++beg)), end, dest);
            }
            else if (bom32 == 0xfffe0000)
            {
                CharsetConverter<To, Encoding::UTF32LE>().ConvertBytes(++(++(++(++beg))), end, dest);
            }
            else if (bom32 == 0x0000fffe)
            {
                CharsetConverter<To, Encoding::UTF32BE>().ConvertBytes(++(++(++(++beg))), end, dest);
            }
            else
            {
                auto bom16 = *binary::ByteIterator<binary::BigEndian, 2, Iter1>(beg, end);

                if (bom16 == 0xfffe)
                {
                    CharsetConverter<To, Encoding::UTF16LE>().ConvertBytes(++(++beg), end, dest);
                }
                else if (bom16 == 0xfeff)
                {
                    CharsetConverter<To, Encoding::UTF16BE>().ConvertBytes(++(++beg), end, dest);
                }
                else
                {
                    CharsetConverter<To, Encoding::ANSI>().ConvertBytes(beg, end, dest);
                }
            }
        }

        template<typename Iter1, typename Iter2>
        void ConvertBytesToUnits(Iter1 beg, Iter1 end, Iter2 dest)
        {
            assert (From != Encoding::Unknown);

            typedef typename details::EncodingTraits<From>::template ByteIterator<Iter1> ByteIter;

            auto unitBeg = ByteIter(beg, end);
            auto unitEnd = ByteIter(end, end);

            this->ConvertUnitsToUnits(unitBeg, unitEnd, dest);
        }

        template<typename Iter1, typename Iter2>
        void ConvertUnitsToUnits(Iter1 beg, Iter1 end, Iter2 dest)
        {
            assert (From != Encoding::Unknown);

            details::EncodingTraits<From> from;
            details::EncodingTraits<To> to;

            while (beg != end)
            {
                uint32_t codePoint;

                beg = from.From(codePoint, beg, end);

                dest = to.To(codePoint, dest);
            }
        }
    };
} }
