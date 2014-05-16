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
    class CsvRowWriter
    {
    private:

        std::vector<std::string>& values;

    private:

        CsvRowWriter(std::vector<std::string>& values)
            : values(values)
        {
        }

    public:

        CsvRowWriter(const CsvRowWriter& other)
            : values(other.values)
        {
        }

        CsvRowWriter(CsvRowWriter&& other)
            : values(other.values)
        {
        }

        CsvRowWriter& operator = (const CsvRowWriter& other)
        {
            this->values = other.values;
            return *this;
        }

        CsvRowWriter& operator = (CsvRowWriter&& other)
        {
            this->values = other.values;
            return *this;
        }

    public:

        template<typename T>
        CsvRowWriter& operator << (const T& value)
        {
            std::ostringstream stream;
            stream << value;

            this->values.push_back(stream.str());

            return *this;
        }

    friend class CsvFile;
    };
} }
