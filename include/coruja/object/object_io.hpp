
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/object/object.hpp"
#include "coruja/support/type_traits.hpp"

#include <istream>
#include <ostream>
#include <utility>

namespace coruja {

template<typename CharType, typename CharTrait, typename ObservableObject>
inline typename std::enable_if<
    is_observable_object<ObservableObject>::value,
    std::basic_ostream<CharType, CharTrait>&
>::type
operator<<(std::basic_ostream<CharType, CharTrait>& out,
           const ObservableObject& obj)
{
    out << obj.get();
    return out;
}

template<typename CharType, typename CharTrait, typename T>
inline std::basic_istream<CharType, CharTrait>&
operator>>(std::basic_istream<CharType, CharTrait>& in, object<T>& obj)
{
    T tmp;
    in >> tmp;
    obj = std::move(tmp);
    return in;
}
    
}
