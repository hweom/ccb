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

#include <ccb/tree/TreeArray.hpp>
#include <ccb/tree/TreeMap.hpp>
#include <ccb/tree/TreeValue.hpp>

namespace ccb { namespace tree
{
    class JsonTreeSerializer
    {
    public:

        void Serialize(const TreeNode& node, std::wostream& stream)
        {
            this->SerializeNode(node, stream);
        }

        std::unique_ptr<TreeNode> Deserialize(std::wistream& stream)
        {
            return this->DeserializeNode(stream);
        }

    private:

        void SerializeNode(const TreeNode& node, std::wostream& stream)
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

        void SerializeMap(const TreeMap& map, std::wostream& stream)
        {
            bool firstItem = true;
            stream << L"{";
            for (const auto& pair : map.GetNodes())
            {
                if (!firstItem)
                {
                    stream << L", ";
                }

                stream << std::endl;

                stream << L"\"" << pair.first << L"\": ";

                this->SerializeNode(*pair.second, stream);
            }

            stream << std::endl << L"}";
        }

        void SerializeArray(const TreeArray& array, std::wostream& stream)
        {
            stream << L"[" << std::endl;
            for (const auto& node : array.GetNodes())
            {
                this->SerializeNode(*node, stream);

                stream << L", " << std::endl;
            }

            stream << L"]";
        }

        void SerializeValue(const TreeValue& value, std::wostream& stream)
        {
            stream << L"\"" << value.GetString() << L"\"";
        }

        std::unique_ptr<TreeNode> DeserializeNode(std::wistream& stream)
        {
            this->SkipWhitespaces(stream);

            if (!stream)
            {
                throw std::runtime_error("Unexpected end of file while looking for node value");
            }

            auto c = stream.peek();
            if (c == L'{')
            {
                return this->DeserializeMap(stream);
            }
            else if (c == L'[')
            {
                return this->DeserializeArray(stream);
            }
            else
            {
                return this->DeserializeValue(stream);
            }
        }

        std::unique_ptr<TreeMap> DeserializeMap(std::wistream& stream)
        {
            auto map = std::unique_ptr<TreeMap>(new TreeMap());

            this->SkipWhitespaces(stream);

            if (stream.get() != '{')
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
                    stream.get();
                    break;
                }

                if (!firstItem)
                {
                    if (stream.get() != ',')
                    {
                        throw std::runtime_error("Missing comma");
                    }

                    this->SkipWhitespaces(stream);
                }

                if (stream.get() != '"')
                {
                    throw std::runtime_error("Missing field name");
                }

                wchar_t c;
                std::wstring fieldName;
                while ((c = stream.get()) != '"')
                {
                    fieldName.push_back(c);
                }

                if (map->HasNode(fieldName))
                {
                    throw std::runtime_error("Duplicate field: " + std::string(fieldName.begin(), fieldName.end()));
                }

                this->SkipWhitespaces(stream);

                if (stream.get() != ':')
                {
                    throw std::runtime_error("Bad JSON structure");
                }

                map->Set(fieldName, this->DeserializeNode(stream));

                firstItem = false;
            }

            return map;
        }

        std::unique_ptr<TreeArray> DeserializeArray(std::wistream& stream)
        {
            auto array = std::unique_ptr<TreeArray>(new TreeArray());

            this->SkipWhitespaces(stream);

            if (stream.get() != '[')
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
                    stream.get();
                    break;
                }
                else
                {
                    // Not the first value - must be a comma.
                    if (!firstItem)
                    {
                        if (stream.get() != ',')
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

        std::unique_ptr<TreeValue> DeserializeValue(std::wistream& stream)
        {
            std::wstring strValue;

            this->SkipWhitespaces(stream);

            wchar_t c = stream.get();

            // Start of a string value.
            if (c == '"')
            {
                while ((c = stream.get()) != '"')
                {
                    strValue.push_back(c);
                }
            }
            else
            {
                strValue.push_back(c);
                c = stream.peek();
                while (!iswspace(c) && (c != L','))
                {
                    stream.get();
                    strValue.push_back(towlower(c));
                    c = stream.peek();
                }
            }

            return std::unique_ptr<TreeValue>(new TreeValue(strValue));
        }

        void SkipWhitespaces(std::wistream& stream)
        {
            while (stream && iswspace(stream.peek()))
            {
                stream.get();
            }
        }
    };
} }
