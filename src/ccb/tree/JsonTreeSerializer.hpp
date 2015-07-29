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

#include <istream>
#include <ostream>

#include <ccb/charset/CharsetConverter.hpp>
#include <ccb/tree/TreeArray.hpp>
#include <ccb/tree/TreeMap.hpp>
#include <ccb/tree/TreeValue.hpp>

namespace ccb { namespace tree
{
    class JsonTreeSerializer
    {
    private:

        charset::CharsetConverter<charset::Encoding::UTF8, charset::Encoding::UTF32LE> writeConverter;

        charset::CharsetConverter<charset::Encoding::UTF32LE, charset::Encoding::UTF8> readConverter;

    public:

        void Serialize(const TreeNode& node, std::ostream& stream)
        {
            // Put UTF8 BOM
            stream << static_cast<uint8_t>(0xef) << static_cast<uint8_t>(0xbb) << static_cast<uint8_t>(0xbf);

            this->SerializeNode(node, stream);
        }

        std::unique_ptr<TreeNode> Deserialize(std::istream& stream)
        {
            // Skip BOM, if present.
            this->SkipBom(stream);

            return this->DeserializeNode(stream);
        }

    private:

        void SerializeNode(const TreeNode& node, std::ostream& stream)
        {
            if (node.GetType() == TreeNodeType::Value)
            {
                this->SerializeValue(static_cast<const TreeValue&>(node), stream);
            }
            else if (node.GetType() == TreeNodeType::Array)
            {
                this->SerializeArray(static_cast<const TreeArray&>(node), stream);
            }
            else if (node.GetType() == TreeNodeType::Map)
            {
                this->SerializeMap(static_cast<const TreeMap&>(node), stream);
            }
            else
            {
                throw std::logic_error("Unknown tree node type");
            }
        }

        void SerializeMap(const TreeMap& map, std::ostream& stream)
        {
            bool firstItem = true;
            stream << "{";
            for (const auto& pair : map.GetNodes())
            {
                if (!firstItem)
                {
                    stream << ", ";
                }

                stream << std::endl;

                stream << "\"";

                this->writeConverter.Convert(pair.first.begin(), pair.first.end(), std::ostream_iterator<char>(stream));

                stream << "\": ";

                this->SerializeNode(*pair.second, stream);

                firstItem = false;
            }

            stream << std::endl << "}";
        }

        void SerializeArray(const TreeArray& array, std::ostream& stream)
        {
            stream << "[" << std::endl;
            for (const auto& node : array.GetNodes())
            {
                this->SerializeNode(*node, stream);

                stream << ", " << std::endl;
            }

            stream << "]";
        }

        void SerializeValue(const TreeValue& value, std::ostream& stream)
        {
            stream << "\"";

            this->writeConverter.Convert(value.GetString().begin(), value.GetString().end(), std::ostream_iterator<char>(stream));

            stream << "\"";
        }

        std::unique_ptr<TreeNode> DeserializeNode(std::istream& stream)
        {
            this->SkipWhitespaces(stream);

            if (!stream)
            {
                throw std::runtime_error("Unexpected end of file while looking for node value");
            }

            auto c = stream.peek();
            if (c == '{')
            {
                return this->DeserializeMap(stream);
            }
            else if (c == '[')
            {
                return this->DeserializeArray(stream);
            }
            else
            {
                return this->DeserializeValue(stream);
            }
        }

        std::unique_ptr<TreeMap> DeserializeMap(std::istream& stream)
        {
            auto map = std::unique_ptr<TreeMap>(new TreeMap());

            this->SkipWhitespaces(stream);

            if (this->GetSymbol(stream) != '{')
            {
                throw std::runtime_error("Not a JSON object");
            }

            bool firstItem = true;

            while (true)
            {
                this->SkipWhitespaces(stream);

                // End of object.
                if (stream.peek() == '}')
                {
                    this->GetSymbol(stream);
                    break;
                }

                if (!firstItem)
                {
                    if (this->GetSymbol(stream) != ',')
                    {
                        throw std::runtime_error("Missing comma");
                    }

                    this->SkipWhitespaces(stream);
                }

                if (this->GetSymbol(stream) != '"')
                {
                    throw std::runtime_error("Missing field name");
                }

                wchar_t c;
                std::wstring fieldName;
                while ((c = this->GetSymbol(stream)) != '"')
                {
                    fieldName.push_back(c);
                }

                if (map->HasNode(fieldName))
                {
                    throw std::runtime_error("Duplicate field: " + std::string(fieldName.begin(), fieldName.end()));
                }

                this->SkipWhitespaces(stream);

                if (this->GetSymbol(stream) != ':')
                {
                    throw std::runtime_error("Bad JSON structure");
                }

                map->Set(fieldName, this->DeserializeNode(stream));

                firstItem = false;
            }

            return map;
        }

        std::unique_ptr<TreeArray> DeserializeArray(std::istream& stream)
        {
            auto array = std::unique_ptr<TreeArray>(new TreeArray());

            this->SkipWhitespaces(stream);

            if (this->GetSymbol(stream) != '[')
            {
                throw std::runtime_error("Not a JSON array");
            }

            bool firstItem = true;

            while (true)
            {
                this->SkipWhitespaces(stream);

                wchar_t c = stream.peek();

                // End of array.
                if (c == ']')
                {
                    this->GetSymbol(stream);
                    break;
                }
                else
                {
                    // Not the first value - must be a comma.
                    if (!firstItem)
                    {
                        if (this->GetSymbol(stream) != ',')
                        {
                            throw std::runtime_error("Missing comma");
                        }
                    }

                    array->Add(this->DeserializeNode(stream));
                }

                firstItem = false;
            }

            return array;
        }

        std::unique_ptr<TreeValue> DeserializeValue(std::istream& stream)
        {
            std::string strValue;

            this->SkipWhitespaces(stream);

            wchar_t c = this->GetSymbol(stream);

            // Start of a string value.
            if (c == '"')
            {
                while ((c = this->GetSymbol(stream)) != '"')
                {
                    strValue.push_back(c);

                    if (!stream)
                    {
                        throw std::runtime_error("Unexpected end of stream");
                    }
                }
            }
            else
            {
                strValue.push_back(c);
                c = stream.peek();
                while (!isspace(c) && (c != ','))
                {
                    c = this->GetSymbol(stream);
                    strValue.push_back(tolower(c));
                    c = stream.peek();
                }
            }

            std::wstring wideStringValue;
            this->readConverter.Convert(strValue.begin(), strValue.end(), std::back_inserter(wideStringValue));

            return std::unique_ptr<TreeValue>(new TreeValue(wideStringValue));
        }

        wchar_t GetSymbol(std::istream& stream)
        {
            return stream.get();
        }

        void SkipWhitespaces(std::istream& stream)
        {
            while (stream && isspace(stream.peek()))
            {
                stream.get();
            }
        }

        void SkipBom(std::istream& stream)
        {
            if (!stream || (stream.peek() != 0xef))
            {
                return;
            }

            stream.get();

            if (!stream || (stream.peek() != 0xbb))
            {
                return;
            }

            stream.get();

            if (!stream || (stream.peek() != 0xbf))
            {
                return;
            }

            stream.get();
        }
    };
} }
