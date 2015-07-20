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

#include <cstdlib>
#include <limits>
#include <type_traits>

namespace ccb { namespace meta
{
    namespace details
    {
        struct NullType {};
    }

    struct Npos : std::integral_constant<size_t, std::numeric_limits<size_t>::max()>{};

    template<typename... Types>
    struct TypeList
    {
        using Head = details::NullType;
        using Tail = details::NullType;
    };

    template<typename T, typename... Types>
    struct TypeList<T, Types...>
    {
        using Head = T;
        using Tail = TypeList<Types...>;
    };

    template<typename TypeList>
    struct Front
    {
        using type = typename TypeList::Head;
    };

    template<typename T>
    struct Back
    {
        using type = T;
    };

    template<typename... Types>
    struct Back<TypeList<Types...>>
    {
        using type = typename std::conditional<
            std::is_same<typename TypeList<Types...>::Tail, details::NullType>::value,
            typename TypeList<Types...>::Head,
            typename Back<typename TypeList<Types...>::Tail>::type>::type;
    };

    template<typename T, typename TypeList>
    struct Next
    {
    };

    template<typename T, typename... Types>
    struct Next<T, TypeList<Types...>>
    {
        using type = typename std::conditional<
            std::is_same<typename Front<TypeList<Types...>>::type, T>::value,
            typename Front<typename TypeList<Types...>::Tail>::type,
            typename Next<T, typename TypeList<Types...>::Tail>::type>::type;
    };

    template<typename T, typename TypeList>
    struct Contains : std::false_type {};

    template<typename ...Args>
    struct Contains<details::NullType, Args...> : std::false_type {};

    template<typename T, typename ...Args>
    struct Contains<T, TypeList<Args...>> : std::integral_constant<bool,
        std::is_same<typename TypeList<Args...>::Head, T>::value ||
        Contains<T, typename TypeList<Args...>::Tail>::value> { };

    namespace details
    {
        template<typename T, unsigned int IndexFrom, typename TypeList>
        struct FindHelper : std::integral_constant<unsigned int, 0> {};

        template<typename T, unsigned int IndexFrom>
        struct FindHelper<T, IndexFrom, TypeList<>> : std::integral_constant<unsigned int, 0> { };

        template<typename T, unsigned int IndexFrom, typename ...Args>
        struct FindHelper<T, IndexFrom, TypeList<Args...>> :
            std::integral_constant<unsigned int,
                std::is_same<typename TypeList<Args...>::Head, T>::value
                ? IndexFrom
                : IndexFrom + 1 + FindHelper<T, IndexFrom, typename TypeList<Args...>::Tail>::value>
        {
        };
    }

    template<typename T, typename TypeList>
    struct Find {};

    template<typename T>
    struct Find<T, TypeList<>> : Npos {};

    template<typename ...Args>
    struct Find<details::NullType, TypeList<Args...>> : Npos {};

    template<typename T, typename ...Args>
    struct Find<T, TypeList<Args...>> : std::integral_constant<
            size_t,
            Contains<T, TypeList<Args...>>::value ? details::FindHelper<T, 0, TypeList<Args...>>::value : Npos::value>
    {
    };
} }
