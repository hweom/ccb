#pragma once

#include <stdexcept>

namespace ccb { namespace filesystem
{
    class NotADirectoryException : public std::runtime_error
    {
    public:

        NotADirectoryException()
            : std::runtime_error("")
        {
        }
    };
} }
