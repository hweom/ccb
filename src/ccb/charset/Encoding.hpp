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

        UTF16 = Encoding::UTF16BE,

        UTF32 = Encoding::UTF32BE

    };
} }
