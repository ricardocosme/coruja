
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <coruja/object/object.hpp>

#include <boost/serialization/split_free.hpp>

namespace boost { namespace serialization {
        
template<typename Archive, typename T>
inline void save(Archive& ar,
                 const coruja::object<T>& o,
                 unsigned int /*version*/)
{ ar << o.observed(); }
 
template<typename Archive, typename T>
inline void load(Archive& ar,
                 coruja::object<T>& o,
                 unsigned int /*version*/)
{
    T observed;
    ar >> observed;
    o = std::move(observed);
}
 
template<typename Archive, typename T>
inline void serialize (Archive& ar,
                       coruja::object<T>& o,
                       unsigned int version)
{ split_free(ar, o, version); }
    
}}
