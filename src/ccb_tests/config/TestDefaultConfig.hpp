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

#include <string>

#include <ccb/config/ConfigSerialization.hpp>

namespace ccb { namespace config
{
    template<typename T>
    class TestDefaultConfig
    {
    private:

        T value;

        T defaultValue;

    public:

        TestDefaultConfig(const T& defaultValue)
            : defaultValue(defaultValue)
        {
        }

        TestDefaultConfig(const T& value, const T& defaultValue)
            : value(value)
            , defaultValue(defaultValue)
        {
        }

    private:

        template<typename Archive>
        void Serialize(Archive& ar)
        {
            CCB_SERIALIZE_DEFAULT(ar, value, this->defaultValue);
        }

    public:

        const T& GetValue() const
        {
            return this->value;
        }

        void SetValue(const T& value)
        {
            this->value = value;
        }

        friend class Access;
    };
} }
