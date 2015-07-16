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

#include <ccb/config/TreeInputArchive.hpp>
#include <ccb/tree/JsonTreeSerializer.hpp>

namespace ccb { namespace config
{
    class JsonInputArchive : public TreeInputArchive
    {
    private:

        std::unique_ptr<TreeMap> tree;

    public:

        JsonInputArchive(const std::string& filename)
        {
            auto stream = std::ifstream(filename);

            auto node = JsonTreeSerializer().Deserialize(stream);

            if (node->GetType() != TreeNodeType::Map)
            {
                throw std::runtime_error("Not a JSON map");
            }

            this->tree = std::unique_ptr<TreeMap>(static_cast<TreeMap*>(node.release()));

            this->SetNode(*this->tree);
        }
    };
} }
