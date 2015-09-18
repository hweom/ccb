#pragma once

namespace ccb { namespace charset
{
    enum class Encoding
    {
        Unknown = 0,

        ANSI,

        UTF8,

        UTF16BE,

        UTF16LE,

        UTF32BE,

        UTF32LE,

        KOI8_R,

        CP_866,

        CP_1251,

        UTF16 = Encoding::UTF16LE,

        UTF32 = Encoding::UTF32LE,

        First = ANSI,

        Last = CP_1251
    };
} }
