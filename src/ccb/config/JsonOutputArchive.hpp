#pragma once

#include <fstream>

#include <ccb/config/TreeOutputArchive.hpp>
#include <ccb/tree/JsonTreeSerializer.hpp>

namespace ccb { namespace config
{
    class JsonOutputArchive : public TreeOutputArchive
    {
    private:

        std::wostream* stream;

        bool ownsStream = false;

    public:

        JsonOutputArchive(const std::string& filename)
        {
            this->stream = new std::wofstream(filename);
            this->ownsStream = true;
        }

        ~JsonOutputArchive()
        {
            JsonTreeSerializer().Serialize(this->GetTree(), *this->stream);

            if (this->ownsStream)
            {
                delete this->stream;
            }
        }

    };
} }
