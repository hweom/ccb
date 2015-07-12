#pragma once

#include <ccb/tree/TreeMap.hpp>
#include <ccb/tree/TreeValue.hpp>

namespace ccb { namespace config
{
    using namespace ccb::tree;

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
            TreeInputArchive subArchive(this->node->GetNode(name));

            value.Serialize(subArchive);
        }

    protected:

        void SetNode(TreeMap& map)
        {
            this->node = &map;
            this->ownNode = false;
        }
    };

    template<>
    inline void TreeInputArchive::Serialize<bool>(bool& value, const std::wstring& name)
    {
        value = this->node->Get<TreeValue>(name).GetString() == L"true";
    }
} }
