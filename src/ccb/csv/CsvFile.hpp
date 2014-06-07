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

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <ccb/csv/CsvRowWriter.hpp>
#include <ccb/csv/CsvRowReader.hpp>
#include <ccb/filesystem/Path.hpp>

namespace ccb { namespace csv
{
    class CsvFile
    {
    private:

        typedef std::vector<std::string> Row;

    public:

        class CsvFileConstIterator
        {
        private:

            std::vector<Row>::const_iterator pos;

        private:

            CsvFileConstIterator(std::vector<Row>::const_iterator pos)
                : pos(pos)
            {
            }

        public:

            CsvRowReader operator * () const
            {
                return CsvRowReader(*this->pos);
            }

            CsvFileConstIterator& operator ++ ()
            {
                this->pos++;
                return *this;
            }

            CsvFileConstIterator operator ++ (int)
            {
                CsvFileConstIterator result = *this;
                this->pos++;
                return result;
            }

            friend bool operator == (const CsvFileConstIterator& i1, const CsvFileConstIterator& i2)
            {
                return i1.pos == i2.pos;
            }

            friend bool operator != (const CsvFileConstIterator& i1, const CsvFileConstIterator& i2)
            {
                return i1.pos != i2.pos;
            }

        friend class CsvFile;
        };

    private:

        static const char DELIMITER = ';';

        std::vector<Row> rows;

    public:

        CsvFile()
        {
        }

        CsvFile(const filesystem::Path& path)
        {
            char buffer[1024];
            std::ifstream stream(path.ToShortString());

            while (stream.good())
            {
                stream.getline(buffer, 1024);

                if (!stream.good())
                {
                    break;
                }

                this->rows.push_back(Row());
                std::string field;
                for (char* c = buffer; *c != 0; c++)
                {
                    if (*c == DELIMITER)
                    {
                        this->rows.back().push_back(field);
                        field.clear();
                    }
                    else
                    {
                        field += *c;
                    }
                }

                if (field.length() > 0)
                {
                    this->rows.back().push_back(field);
                }
            }
        }

    public:

        CsvFileConstIterator begin() const
        {
            return CsvFileConstIterator(this->rows.begin());
        }

        CsvFileConstIterator end() const
        {
            return CsvFileConstIterator(this->rows.end());
        }

        CsvRowWriter Add()
        {
            this->rows.push_back(Row());
            return CsvRowWriter(this->rows.back());
        }

        void Save(const filesystem::Path& path)
        {
            std::ofstream stream(path.ToShortString());

            for (auto& row : this->rows)
            {
                bool first = true;
                for (auto& val : row)
                {
                    if (!first)
                    {
                        stream << ";";
                    }

                    stream << val;

                    first = false;
                }

                stream << std::endl;
            }
        }
    };
} }
