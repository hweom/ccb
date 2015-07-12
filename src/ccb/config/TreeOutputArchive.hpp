#pragma once

#include <ccb/tree/TreeMap.hpp>
#include <ccb/tree/TreeValue.hpp>

namespace ccb { namespace config
{
    using namespace ccb::tree;

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

        template<typename T>
        void Serialize(const T& value, const std::wstring& name)
        {
            auto& subNode = this->node->Set<TreeMap>(name);

            TreeOutputArchive subArchive(subNode);

            value.Serialize(subArchive);
        }

        TreeMap& GetTree()
        {
            return *this->node;
        }
    };

    template<>
    inline void TreeOutputArchive::Serialize<bool>(const bool& value, const std::wstring& name)
    {
        this->node->Set<TreeValue>(name).SetString(value ? L"true" : L"false");
    }
} }
