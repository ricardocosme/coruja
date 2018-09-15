
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/support/type_traits.hpp"

namespace coruja {

//Object<T1> vs Object<T2> cases    
template<typename Object1, typename Object2>
inline enable_if_t<
    is_observable_object<Object1>::value
    && is_observable_object<Object2>::value,
    bool
>
operator==(const Object1& lhs, const Object2& rhs)
{ return lhs.get() == rhs.get(); }

template<typename Object1, typename Object2>
inline enable_if_t<
    is_observable_object<Object1>::value
    && is_observable_object<Object2>::value,
    bool
>
operator!=(const Object1& lhs, const Object2& rhs)
{ return !(lhs == rhs); }
    
template<typename Object1, typename Object2>
inline enable_if_t<
    is_observable_object<Object1>::value
    && is_observable_object<Object2>::value,
    bool
>
operator<(const Object1& lhs, const Object2& rhs)
{ return lhs.get() < rhs.get(); }
    
template<typename Object1, typename Object2>
inline enable_if_t<
    is_observable_object<Object1>::value
    && is_observable_object<Object2>::value,
    bool
>
operator>(const Object1& lhs, const Object2& rhs)
{ return rhs < lhs; }
    
template<typename Object1, typename Object2>
inline enable_if_t<
    is_observable_object<Object1>::value
    && is_observable_object<Object2>::value,
    bool
>
operator<=(const Object1& lhs, const Object2& rhs)
{ return !(rhs < lhs); }
    
template<typename Object1, typename Object2>
inline enable_if_t<
    is_observable_object<Object1>::value
    && is_observable_object<Object2>::value,
    bool
>
operator>=(const Object1& lhs, const Object2& rhs)
{ return !(lhs.get() < rhs.get()); }
    
// T vs Object<T> cases    
template<typename Object>
inline enable_if_t<
    is_observable_object<Object>::value, bool>
operator==(const typename Object::observed_t& lhs, const Object& rhs)
{ return lhs == rhs.get(); }
    
template<typename Object>
inline enable_if_t<
    is_observable_object<Object>::value, bool>
operator!=(const typename Object::observed_t& lhs, const Object& rhs)
{ return !(lhs == rhs); }
    
template<typename Object>
inline enable_if_t<
    is_observable_object<Object>::value, bool>
operator<(const typename Object::observed_t& lhs, const Object& rhs)
{ return lhs < rhs.get(); }
    
template<typename Object>
inline enable_if_t<
    is_observable_object<Object>::value, bool>
operator>(const typename Object::observed_t& lhs, const Object& rhs)
{ return rhs.get() < lhs; }
    
template<typename Object>
inline enable_if_t<
    is_observable_object<Object>::value, bool>
operator<=(const typename Object::observed_t& lhs, const Object& rhs)
{ return !(rhs.get() < lhs); }
    
template<typename Object>
inline enable_if_t<
    is_observable_object<Object>::value, bool>
operator>=(const typename Object::observed_t& lhs, const Object& rhs)
{ return !(lhs < rhs); }
    
// Object<T> vs T cases    
template<typename Object>
inline enable_if_t<
    is_observable_object<Object>::value, bool>
operator==(const Object& lhs, const typename Object::observed_t& rhs)
{ return lhs.get() == rhs; }
    
template<typename Object>
inline enable_if_t<
    is_observable_object<Object>::value, bool>
operator!=(const Object& lhs, const typename Object::observed_t& rhs)
{ return !(lhs == rhs); }
    
template<typename Object>
inline enable_if_t<
    is_observable_object<Object>::value, bool>
operator<(const Object& lhs, const typename Object::observed_t& rhs)
{ return lhs.get() < rhs; }
    
template<typename Object>
inline enable_if_t<
    is_observable_object<Object>::value, bool>
operator>(const Object& lhs, const typename Object::observed_t& rhs)
{ return rhs < lhs; }
    
template<typename Object>
inline enable_if_t<
    is_observable_object<Object>::value, bool>
operator<=(const Object& lhs, const typename Object::observed_t& rhs)
{ return !(rhs < lhs); }
    
template<typename Object>
inline enable_if_t<
    is_observable_object<Object>::value, bool>
operator>=(const Object& lhs, const typename Object::observed_t& rhs)
{ return !(lhs < rhs); }
    
}
