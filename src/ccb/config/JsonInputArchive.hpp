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
            auto stream = std::wifstream(filename);

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
