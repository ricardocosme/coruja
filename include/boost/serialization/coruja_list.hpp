
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <coruja/container/list.hpp>
#include <boost/serialization/coruja_sequence.hpp>

namespace boost { namespace serialization {

template<typename Archive,
         typename T,
         typename Allocator,
         template <typename, typename> class Observed,
         typename Derived,
         template <typename> class Signal>
inline void save(Archive& ar,
                 const coruja::list<
                     T,
                     Allocator,
                     Observed,
                     Derived,
                     Signal>& o,
                 unsigned int version)
{ coruja::sequence_serialization::save(ar, o, version); }
 
template<typename Archive,
         typename T,
         typename Allocator,
         template <typename, typename> class Observed,
         typename Derived,
         template <typename> class Signal>
inline void load(Archive& ar,
                 coruja::list<
                     T,
                     Allocator,
                     Observed,
                     Derived,
                     Signal>& o,
                 unsigned int version)
{ coruja::sequence_serialization::load(ar, o, version); }
    
template<typename Archive,
         typename T,
         typename Allocator,
         template <typename, typename> class Observed,
         typename Derived,
         template <typename> class Signal>
inline void serialize (Archive& ar,
                       coruja::list<
                           T,
                           Allocator,
                           Observed,
                           Derived,
                           Signal>& o,
                       unsigned int version)
{ split_free(ar, o, version); }
    
}}
