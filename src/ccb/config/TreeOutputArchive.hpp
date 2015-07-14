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

#include <ccb/config/ConfigSerialization.hpp>
#include <ccb/tree/TreeMap.hpp>
#include <ccb/tree/TreeValue.hpp>

namespace ccb { namespace config
{
    using namespace ccb::tree;

    namespace details
    {
        template<typename Archive, typename T, class Enable = void>
        struct OutputSerialize
        {
            void operator () (TreeMap& map, T& value, const std::wstring& name)
            {
                Access access;

                Archive subArchive(map.Set<TreeMap>(name));

                access.Serialize(subArchive, value);
            }
        };

        template<typename Archive>
        struct OutputSerialize<Archive, bool>
        {
            void operator () (TreeMap& map, bool& value, const std::wstring& name)
            {
                map.Set<TreeValue>(name).SetString(value ? L"true" : L"false");
            }
        };

        template<typename Archive>
        struct OutputSerialize<Archive, int8_t>
        {
            void operator () (TreeMap& map, int8_t& value, const std::wstring& name)
            {
                std::wostringstream stream;
                stream << static_cast<int32_t>(value);

                map.Set<TreeValue>(name).SetString(stream.str());
            }
        };

        template<typename Archive>
        struct OutputSerialize<Archive, uint8_t>
        {
            void operator () (TreeMap& map, uint8_t& value, const std::wstring& name)
            {
                std::wostringstream stream;
                stream << static_cast<int32_t>(value);

                map.Set<TreeValue>(name).SetString(stream.str());
            }
        };

        template<typename Archive, typename U>
        struct OutputSerialize<
            Archive,
            U,
            typename std::enable_if<std::is_arithmetic<U>::value && !std::is_same<U, int8_t>::value && !std::is_same<U, uint8_t>::value>::type>
        {
            void operator () (TreeMap& map, U& value, const std::wstring& name)
            {
                std::wostringstream stream;
                stream << value;

                map.Set<TreeValue>(name).SetString(stream.str());
            }
        };

        template<typename Archive, typename Char>
        struct OutputSerialize<Archive, std::basic_string<Char>>
        {
            void operator () (TreeMap& map, std::basic_string<Char>& value, const std::wstring& name)
            {
                map.Set<TreeValue>(name).SetString(std::wstring(value.begin(), value.end()));
            }
        };
    }

    class TreeOutputArchive
    {
    private:

        TreeMap* node;

        bool ownNode = false;

    public:

        TreeOutputArchive()
        {
            this->node = new TreeMap();
            this->ownNode = true;
        }

        TreeOutputArchive(TreeMap& node)
            : node(&node)
        {
        }

        ~TreeOutputArchive()
        {
            if (this->ownNode)
            {
                delete this->node;
            }
        }

    public:

        bool IsOutput() const
        {
            return true;
        }

        template<typename T>
        void Serialize(T& value, const std::wstring& name)
        {
            details::OutputSerialize<TreeOutputArchive, T>()(*this->node, value, name);
        }

        template<typename T>
        void Serialize(T& value, const std::wstring& name, const T& defaultValue)
        {
            details::OutputSerialize<TreeOutputArchive, T>()(*this->node, value, name);
        }

        TreeMap& GetTree()
        {
            return *this->node;
        }
    };
} }
