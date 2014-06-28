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

#include <iostream>
#include <cstring>
#include <cxxabi.h>
#include <execinfo.h>
#include <signal.h>
#include <sys/wait.h>

namespace ccb
{
    class CrashHandler
    {
    private:

        static const size_t MAX_STACK_DEPTH = 100;

        static const size_t MAX_LINE_LENGTH = 400;

    public:

        static void Init()
        {
            signal(SIGABRT, &CrashHandler::ProcessCrash);
            signal(SIGSEGV, &CrashHandler::ProcessCrash);
        }

        static void Crash()
        {
            volatile int *p = reinterpret_cast<volatile int*>(0);
            *p = 0x1337D00D;
        }

    private:

        static void ProcessCrash(int sig)
        {
            size_t stack_depth;
            void *stack_addrs[MAX_STACK_DEPTH];
            char **stack_strings;

            stack_depth = backtrace(stack_addrs, MAX_STACK_DEPTH);
            stack_strings = backtrace_symbols(stack_addrs, stack_depth);

            for (size_t i = 1; i < stack_depth; i++)
            {
                size_t sz = MAX_LINE_LENGTH;
                char *function = static_cast<char*>(malloc(sz));
                char *begin = 0, *end = 0;

                // find the parentheses and address offset surrounding the mangled name
                for (char *j = stack_strings[i]; *j; ++j)
                {
                    if (*j == '(')
                    {
                        begin = j;
                    }
                    else if (*j == '+')
                    {
                        end = j;
                    }
                }

                if (begin && end)
                {
                    *begin++ = 0;
                    *end = 0;

                    // found our mangled name, now in [begin, end)

                    int status;
                    char *ret = abi::__cxa_demangle(begin, function, &sz, &status);
                    if (ret)
                    {
                        // return value may be a realloc() of the input
                        function = ret;
                    }
                    else
                    {
                        // demangling failed, just pretend it's a C function with no args
                        std::strncpy(function, begin, sz);
                        std::strncat(function, "()", sz);
                        function[sz-1] = 0;
                    }

                    std::cout << "    " << stack_strings[i] << ":" << function << std::endl;
                }
                else
                {
                    // didn't find the mangled name, just print the whole line
                    std::cout << "    " << stack_strings[i] << std::endl;
                }

                free(function);
            }

            free(stack_strings); // malloc()ed by backtrace_symbols

            exit(1);
        }
    };
}
