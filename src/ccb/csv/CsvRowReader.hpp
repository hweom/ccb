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

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace ccb { namespace csv
{
    class CsvRowReader
    {
    private:

        const std::vector<std::string>* values;

        std::vector<std::string>::const_iterator pos;

    private:

        CsvRowReader(const std::vector<std::string>& values)
            : values(&values)
            , pos(values.begin())
        {
        }

    public:

        CsvRowReader(const CsvRowReader& other)
            : values(other.values)
            , pos(other.pos)
        {
        }

        CsvRowReader(CsvRowReader&& other)
            : values(other.values)
            , pos(other.pos)
        {
        }

        CsvRowReader& operator = (const CsvRowReader& other)
        {
            this->values = other.values;
            this->pos = other.pos;
            return *this;
        }

        CsvRowReader& operator = (CsvRowReader&& other)
        {
            this->values = other.values;
            this->pos = other.pos;
            return *this;
        }

    public:

        template<typename T>
        CsvRowReader& operator >> (T& value)
        {
            if (this->pos == this->values->end())
            {
                throw std::runtime_error("Reached the end of CSV row");
            }

            std::istringstream stream(*this->pos);
            stream >> value;

            this->pos++;

            return *this;
        }

    friend class CsvFile;
    };
} }
