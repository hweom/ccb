#pragma once

#include <string>

#include <ccb/charset/CharsetConverter.hpp>
#include <ccb/charset/Encoding.hpp>
#include <ccb/charset/EncodingAlias.hpp>

namespace ccb { namespace charset
{
    namespace details
    {
        template<typename Iter, Encoding utf, Encoding enc>
        struct ConvertSearcher
        {
            std::wstring operator ()(Iter beg, Iter end, const std::string& fromEncoding)
            {
                if (EncodingAlias().IsAliasFor(enc, fromEncoding))
                {
                    std::wstring result;

                    CharsetConverter<utf, enc>().ConvertBytes(beg, end, std::back_inserter(result));

                    return result;
                }
                else
                {
                    return ConvertSearcher<Iter, utf, static_cast<Encoding>(static_cast<unsigned>(enc) + 1)>()(beg, end, fromEncoding);
                }
            }
        };

        template<typename Iter, Encoding utf>
        struct ConvertSearcher<Iter, utf, Encoding::Last>
        {
            std::wstring operator ()(Iter beg, Iter end, const std::string& fromEncoding)
            {
                if (EncodingAlias().IsAliasFor(Encoding::Last, fromEncoding))
                {
                    std::wstring result;

                    CharsetConverter<utf, Encoding::Last>().ConvertBytes(beg, end, std::back_inserter(result));

                    return result;
                }
                else
                {
                    std::wstring result;

                    CharsetConverter<utf, Encoding::Unknown>().ConvertBytes(beg, end, std::back_inserter(result));

                    return result;
                }
            }
        };
    }

    template<typename Iter>
    std::wstring ConvertBytesToPlatformUtf(Iter beg, Iter end, const std::string& fromEncoding)
    {
#ifdef _WIN32
        return details::ConvertSearcher<Iter, Encoding::UTF16, Encoding::First>()(beg, end, fromEncoding);
#else
        return details::ConvertSearcher<Iter, Encoding::UTF32, Encoding::First>()(beg, end, fromEncoding);
#endif

    }
} }
