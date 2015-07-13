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
        struct InputSerialize
        {
            void operator () (TreeMap& map, T& value, const std::wstring& name)
            {
                Access access;

                auto& subNode = map.GetNode(name);

                if (subNode.GetType() != TreeNodeType::Map)
                {
                    throw std::runtime_error("Expected field to be a map");
                }

                Archive subArchive(static_cast<TreeMap&>(subNode));

                access.Serialize(subArchive, value);
            }
        };

        template<typename Archive>
        struct InputSerialize<Archive, bool, void>
        {
            void operator () (TreeMap& map, bool& value, const std::wstring& name)
            {
                value = map.Get<TreeValue>(name).GetString() == L"true";
            }
        };

        template<typename Archive>
        struct InputSerialize<Archive, int8_t, void>
        {
            void operator () (TreeMap& map, int8_t& value, const std::wstring& name)
            {
                int32_t v;

                std::wistringstream stream(map.Get<TreeValue>(name).GetString());
                stream >> v;

                value = static_cast<int8_t>(v);
            }
        };

        template<typename Archive>
        struct InputSerialize<Archive, uint8_t, void>
        {
            void operator () (TreeMap& map, uint8_t& value, const std::wstring& name)
            {
                uint32_t v;

                std::wistringstream stream(map.Get<TreeValue>(name).GetString());
                stream >> v;

                value = static_cast<uint8_t>(v);
            }
        };

        template<typename Archive, typename U>
        struct InputSerialize<
            Archive,
            U,
            typename std::enable_if<std::is_arithmetic<U>::value && !std::is_same<U, int8_t>::value && !std::is_same<U, uint8_t>::value>::type>
        {
            void operator () (TreeMap& map, U& value, const std::wstring& name)
            {
                std::wistringstream stream(map.Get<TreeValue>(name).GetString());
                stream >> value;
            }
        };

        template<typename Archive, typename Char>
        struct InputSerialize<Archive, std::basic_string<Char>, void>
        {
            void operator () (TreeMap& map, std::basic_string<Char>& value, const std::wstring& name)
            {
                const auto& str = map.Get<TreeValue>(name).GetString();
                value = std::basic_string<Char>(str.begin(), str.end());
            }
        };
    }

    class TreeInputArchive
    {
    private:

        TreeMap* node = nullptr;

        bool ownNode = false;

    public:

        TreeInputArchive()
        {
        }

        TreeInputArchive(TreeMap& node)
            : node(&node)
            , ownNode(false)
        {
        }

    public:

        template<typename T>
        void Serialize(T& value, const std::wstring& name)
        {
            details::InputSerialize<TreeInputArchive, T>()(*this->node, value, name);
        }

    protected:

        void SetNode(TreeMap& map)
        {
            this->node = &map;
            this->ownNode = false;
        }
    };

} }
