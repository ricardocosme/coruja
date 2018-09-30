
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <coruja/boost_optional.hpp>

#include <boost/serialization/split_free.hpp>
#include <boost/serialization/serialization.hpp>

namespace coruja { namespace serialization {
                
struct optional
{        
    template<typename Archive, typename T>
    static void save(Archive& ar,
                     const coruja::optional<T>& o,
                     unsigned int /*version*/)
    {
        ar << o.observed();
    }
 
    template<typename Archive, typename T>
    static void load(Archive& ar,
                     coruja::optional<T>& o,
                     unsigned int version)
    {
        typename coruja::optional<T>::observed_t tmp;
        ar >> tmp;
        o._observed = std::move(tmp);
    }
};
        
}}

namespace boost { namespace serialization {
                        
template<typename Archive, typename T>
inline void save(Archive& ar,
                 const coruja::optional<T>& o,
                 unsigned int version)
{
    ::coruja::serialization::optional::save(ar, o, version);
}
 
template<typename Archive, typename T>
inline void load(Archive& ar,
                 coruja::optional<T>& o,
                 unsigned int version)
{
    ::coruja::serialization::optional::load(ar, o, version);
}
 
template<typename Archive, typename T>
inline void serialize(Archive& ar, coruja::optional<T>& o,
                      unsigned int version)
{
    split_free(ar, o, version);
}
    
}}
