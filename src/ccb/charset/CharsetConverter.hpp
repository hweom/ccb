#pragma once

#include <cstdint>

#include <ccb/charset/Encoding.hpp>

namespace ccb { namespace charset
{
    namespace details
    {
        struct BigEndian
        {
            template<typename T, typename Iter>
            Iter GetFromBytes(T& value, Iter beg, Iter end)
            {
                value = 0;

                for (size_t i = 0; (i < sizeof(T)) && (beg != end); i++, beg++)
                {
                    auto byte = static_cast<uint8_t>((*beg) & 0xff);
                    value = (value << 8) | byte;
                }

                return beg;
            }

            template<typename T, typename Iter>
            T GetFromBytes(Iter beg, Iter end)
            {
                T value = 0;

                this->GetFromBytes(value, beg, end);

                return value;
            }
        };

        struct LittleEndian
        {
            template<typename T, typename Iter>
            Iter GetFromBytes(T& value, Iter beg, Iter end)
            {
                value = 0;

                for (size_t i = 0; (i < sizeof(T)) && (beg != end); i++, beg++)
                {
                    auto byte = static_cast<uint8_t>((*beg) & 0xff);
                    value = value | (static_cast<T>(byte) << (i * 8));
                }

                return beg;
            }

            template<typename T, typename Iter>
            T GetFromBytes(Iter beg, Iter end)
            {
                T value = 0;

                this->GetFromBytes(value, beg, end);

                return value;
            }
        };

        template<Encoding Enc>
        struct Getter
        {
            template<typename Iter>
            Iter FromBytes (uint32_t& codePoint, Iter begin, Iter end)
            {
                codePoint = static_cast<uint32_t>(*begin);

                return ++begin;
            }
        };

        template<Encoding Enc>
        struct Putter
        {
            template<typename Iter>
            Iter ToUnits (uint32_t codePoint, Iter pos)
            {
                *pos = codePoint;

                return ++pos;
            }
        };

        template<>
        struct Getter<Encoding::UTF8>
        {
            template<typename Iter>
            Iter FromBytes (uint32_t& codePoint, Iter begin, Iter end)
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

                    size_t bytes = 1;
                    while ((bytes < 8) && ((b0 & (0x100 >> bytes)) != 0))
                    {
                        b0 &= ~(0x100 >> bytes);
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
        };

        struct Utf16Getter
        {
            template<typename Iter, typename Endian>
            Iter FromBytes (uint32_t& codePoint, Iter begin, Iter end)
            {
                uint16_t u0, u1;
                begin = Endian().GetFromBytes(u0, begin, end);

                if ((u0 < 0xd800) || (u0 >= 0xdc00))
                {
                    codePoint = u0;
                    return begin;
                }

                auto savedPos = begin;

                begin = Endian().GetFromBytes(u1, begin, end);

                if ((u1 < 0xdc00) || (u1 >= 0xe000))
                {
                    codePoint = u0;
                    return savedPos;
                }

                codePoint = (static_cast<uint32_t>(u0 - 0xd800) << 16) | (u1 - 0xdc00);
                return begin;
            }
        };

        template<>
        struct Getter<Encoding::UTF16BE>
        {
            template<typename Iter>
            Iter FromBytes (uint32_t&  codePoint, Iter begin, Iter end)
            {
                return Utf16Getter().FromBytes<Iter, BigEndian>(codePoint, begin, end);
            }
        };

        template<>
        struct Getter<Encoding::UTF16LE>
        {
            template<typename Iter>
            Iter FromBytes (uint32_t& codePoint, Iter begin, Iter end)
            {
                return Utf16Getter().FromBytes<Iter, LittleEndian>(codePoint, begin, end);
            }
        };

        template<>
        struct Getter<Encoding::UTF32BE>
        {
            template<typename Iter>
            Iter FromBytes (uint32_t& codePoint, Iter begin, Iter end)
            {
                return BigEndian().GetFromBytes(codePoint, begin, end);
            }
        };

        template<>
        struct Getter<Encoding::UTF32LE>
        {
            template<typename Iter>
            Iter FromBytes (uint32_t& codePoint, Iter begin, Iter end)
            {
                return LittleEndian().GetFromBytes(codePoint, begin, end);
            }
        };
    }

    class CharsetConverter
    {
    public:

        template<Encoding To, Encoding From, typename Iter1, typename Iter2>
        void ConvertBytes(Iter1 beg, Iter1 end, Iter2 dest)
        {
            if (From == Encoding::Unknown)
            {
                this->ConvertUtfBytes<To>(beg, end, dest);
            }
            else
            {
                this->ConvertBytesToUnits<To, From, Iter1, Iter2>(beg, end, dest);
            }
        }

    private:

        template<Encoding To, typename Iter1, typename Iter2>
        void ConvertUtfBytes(Iter1 beg, Iter1 end, Iter2 dest)
        {
            auto bom32 = details::BigEndian().GetFromBytes<uint32_t>(beg, end);

            if ((bom32 & 0xffffff00) == 0xefbbbf)
            {
                this->ConvertBytesToUnits<To, Encoding::UTF8, Iter1, Iter2>(++(++(++beg)), end, dest);
            }
            else if (bom32 == 0xfffe0000)
            {
                this->ConvertBytesToUnits<To, Encoding::UTF32LE, Iter1, Iter2>(++(++(++(++beg))), end, dest);
            }
            else if (bom32 == 0x0000fffe)
            {
                this->ConvertBytesToUnits<To, Encoding::UTF32BE, Iter1, Iter2>(++(++(++(++beg))), end, dest);
            }
            else
            {
                auto bom16 = details::BigEndian().GetFromBytes<uint16_t>(beg, end);

                if (bom16 == 0xfffe)
                {
                    this->ConvertBytesToUnits<To, Encoding::UTF16LE, Iter1, Iter2>(++(++beg), end, dest);
                }
                else if (bom16 == 0xfeff)
                {
                    this->ConvertBytesToUnits<To, Encoding::UTF16BE, Iter1, Iter2>(++(++beg), end, dest);
                }

                this->ConvertBytesToUnits<To, Encoding::ANSI, Iter1, Iter2>(beg, end, dest);
            }
        }

        template<Encoding To, Encoding From, typename Iter1, typename Iter2>
        void ConvertBytesToUnits(Iter1 beg, Iter1 end, Iter2 dest)
        {
            assert (From != Encoding::Unknown);

            details::Getter<From> from;
            details::Putter<To> to;

            while (beg != end)
            {
                uint32_t codePoint;

                beg = from.FromBytes(codePoint, beg, end);

                dest = to.ToUnits(codePoint, dest);
            }
        }
    };
} }
