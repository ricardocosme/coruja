
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <coruja/container/vector.hpp>
#include <boost/serialization/coruja_sequence.hpp>

namespace boost { namespace serialization {
        
template<typename T,
         typename Allocator,
         template <typename, typename> class Observed,
         typename Derived,
         template <typename> class Signal>
void pre_load(coruja::vector<
                  T,
                  Allocator,
                  Observed,
                  Derived,
                  Signal>& o,
              typename Observed<T, Allocator>::size_type count)
{ o.reserve(count); }

template<typename Archive,
         typename T,
         typename Allocator,
         template <typename, typename> class Observed,
         typename Derived,
         template <typename> class Signal>
inline void save(Archive& ar,
                 const coruja::vector<
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
                 coruja::vector<
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
                       coruja::vector<
                           T,
                           Allocator,
                           Observed,
                           Derived,
                           Signal>& o,
                       unsigned int version)
{ split_free(ar, o, version); }
    
}}
