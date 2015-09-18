#pragma once

#include <string>
#include <unordered_map>

#include <ccb/charset/Encoding.hpp>

namespace ccb { namespace charset
{
    class EncodingAlias
    {
    public:

        bool IsAliasFor(Encoding encoding, const std::string& alias) const
        {
            auto normalizedAlias = this->NormalizeAlias(alias);

            const auto& map = GetAliasMap();
            auto pos = map.find(normalizedAlias);

            return (pos != map.end()) && (pos->second == encoding);
        }

    private:

        /// Remove all non-letters and non-digits and make remaining symbols lowercase.
        std::string NormalizeAlias(const std::string& alias) const
        {
            std::string result;

            for (auto c : alias)
            {
                if (isalpha(c))
                {
                    result.push_back(tolower(c));
                }
                else if (isdigit(c))
                {
                    result.push_back(c);
                }
            }

            return result;
        }

    private:

        static std::unordered_map<std::string, Encoding>& GetAliasMap()
        {
            static std::unordered_map<std::string, Encoding> map = CreateAliasMap();
            return map;
        }

        static std::unordered_map<std::string, Encoding> CreateAliasMap()
        {
            return std::unordered_map<std::string, Encoding>(
            {
                { "ansi", Encoding::ANSI },
                { "koi8r", Encoding::KOI8_R },
                { "cp866", Encoding::CP_866 },
                { "cp1251", Encoding::CP_1251 },
                { "windows866", Encoding::CP_866 },
                { "windows1251", Encoding::CP_1251 },
                { "utf8", Encoding::UTF8 },
                { "utf16", Encoding::UTF16 },
                { "utf32", Encoding::UTF32 },
            });
        }
    };

} }
