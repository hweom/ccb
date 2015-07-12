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

#include <map>
#include <memory>
#include <string>

#include <ccb/tree/TreeArray.hpp>
#include <ccb/tree/TreeMap.hpp>

namespace ccb { namespace tree
{
    class TreeArray;
    class TreeValue;

    class TreeMap : public TreeNode
    {
    private:

        std::map<std::wstring, std::unique_ptr<TreeNode>> nodes;

    public:

        TreeMap()
        {
        }

    public:

        virtual TreeNodeType GetType() const override
        {
            return TreeNodeType::Map;
        }

        bool HasNode(const std::wstring& name) const
        {
            return this->nodes.count(name) > 0;
        }

        const std::map<std::wstring, std::unique_ptr<TreeNode>>& GetNodes() const
        {
            return this->nodes;
        }

        TreeNode& GetNode(const std::wstring& name)
        {
            auto pos = this->nodes.find(name);
            if (pos == this->nodes.end())
            {
                throw std::runtime_error("No such node");
            }

            return *pos->second;
        }

        template<typename NodeType>
        const typename std::enable_if<std::is_base_of<TreeNode, NodeType>::value, NodeType>::type& Get(const std::wstring& name) const
        {
            auto pos = this->nodes.find(name);
            if (pos == this->nodes.end())
            {
                throw std::runtime_error("No such node");
            }

            auto node = dynamic_cast<const NodeType*>(pos->second.get());
            if (node == nullptr)
            {
                throw std::runtime_error("Is not of requested type");
            }

            return *node;
        }

        template<typename NodeType>
        typename std::enable_if<std::is_base_of<TreeNode, NodeType>::value, NodeType>::type& Set(const std::wstring& name)
        {
            auto pos = this->nodes.find(name);
            if (pos != this->nodes.end())
            {
                this->nodes.erase(pos);
            }

            auto node = new NodeType();
            this->nodes.insert(std::make_pair(name, std::unique_ptr<TreeNode>(node)));
            return *node;
        }

        void Set(const std::wstring& name, std::unique_ptr<TreeNode>&& node)
        {
            this->nodes[name] = std::move(node);
        }
    };
} }
