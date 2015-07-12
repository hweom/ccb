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

#include <memory>
#include <string>
#include <vector>

#include <ccb/tree/TreeValue.hpp>

namespace ccb { namespace tree
{
    class TreeMap;
    class TreeValue;

    class TreeArray : public TreeNode
    {
    private:

        std::vector<std::unique_ptr<TreeNode>> nodes;

    public:

        virtual TreeNodeType GetType() const override
        {
            return TreeNodeType::Array;
        }

        size_t GetSize() const
        {
            return this->nodes.size();
        }

        const std::vector<std::unique_ptr<TreeNode>>& GetNodes() const
        {
            return this->nodes;
        }

        template<typename NodeType>
        const typename std::enable_if<std::is_base_of<TreeNode, NodeType>::value, NodeType>::type& Get(size_t idx) const
        {
            if (this->nodes.size() <= idx)
            {
                throw std::runtime_error("Index out of range");
            }

            auto node = dynamic_cast<const NodeType*>(this->nodes.at(idx).get());
            if (node == nullptr)
            {
                throw std::runtime_error("Is not of requested type");
            }

            return *node;
        }

        template<typename NodeType>
        typename std::enable_if<std::is_base_of<TreeNode, NodeType>::value, NodeType>::type& Set(size_t idx)
        {
            if (this->nodes.size() <= idx)
            {
                throw std::runtime_error("Index out of range");
            }

            auto node = dynamic_cast<const NodeType*>(this->nodes.at(idx).get());
            if (node == nullptr)
            {
                throw std::runtime_error("Is not of requested type");
            }

            return *node;
        }

        template<typename NodeType>
        typename std::enable_if<std::is_base_of<TreeNode, NodeType>::value, NodeType>::type& Add()
        {
            auto node = new NodeType();
            this->nodes.push_back(std::unique_ptr<NodeType>(node));
            return *node;
        }

        void Add(std::unique_ptr<TreeNode>&& node)
        {
            this->nodes.push_back(std::move(node));
        }
    };
} }
