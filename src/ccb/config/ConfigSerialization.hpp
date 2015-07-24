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

#include <functional>

#define CCB_WIDEN(x) L ## x
#define CCB_SERIALIZE(ar, field)  (ar).Serialize((field), CCB_WIDEN(#field))
#define CCB_SERIALIZE_DEFAULT(ar, field, def)  (ar).Serialize((field), CCB_WIDEN(#field), (def))
#define CCB_SERIALIZE_AS(type, ar, field, from, to) ccb::config::ProxySerialize<type>((ar), (field), CCB_WIDEN(#field), (from), (to))
#define CCB_SERIALIZE_AS_DEFAULT(type, ar, field, def, from, to) ccb::config::ProxySerializeDefault<type>((ar), (field), (def), CCB_WIDEN(#field), (from), (to))
#define CCB_SERIALIZE_CAST_DEFAULT(type, ar, field, def) ccb::config::CastSerializeDefault<type>((ar), (field), (def), CCB_WIDEN(#field))

namespace ccb { namespace config
{
    class Access
    {
    public:

        template<typename Archive, typename T>
        void Serialize(Archive& ar, T& value)
        {
            value.Serialize(ar);
        }
    };

    template<typename U, typename Archive, typename T, typename From, typename To>
    inline void ProxySerialize(Archive& ar, T& value, const std::wstring& name, From from, To to)
    {
        if (ar.IsOutput())
        {
            U proxy = to(value);

            ar.Serialize(proxy, name);
        }
        else
        {
            U proxy;

            ar.Serialize(proxy, name);

            value = from(proxy);
        }
    }

    template<typename U, typename Archive, typename T, typename From, typename To>
    inline void ProxySerializeDefault(Archive& ar, T& value, const T& defaultValue, const std::wstring& name, From from, To to)
    {
        if (ar.IsOutput())
        {
            U proxy = static_cast<U>(to(value));

            ar.Serialize(proxy, name);
        }
        else
        {
            U proxy;

            ar.Serialize(proxy, name, static_cast<U>(to(defaultValue)));

            value = from(proxy);
        }
    }

    template<typename U, typename Archive, typename T>
    inline void CastSerializeDefault(Archive& ar, T& value, const T& defaultValue, const std::wstring& name)
    {
        if (ar.IsOutput())
        {
            auto proxy = static_cast<U>(value);

            ar.Serialize(proxy, name);
        }
        else
        {
            U proxy;

            ar.Serialize(proxy, name, static_cast<U>(defaultValue));

            value = static_cast<T>(proxy);
        }
    }
} }
