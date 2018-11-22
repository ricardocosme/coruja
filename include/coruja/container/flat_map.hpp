
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/container/map.hpp"

#include <boost/container/flat_map.hpp>

namespace coruja {

template<typename Key,
         typename T,
         typename Compare = std::less<Key>,
         typename Allocator = std::allocator<std::pair<Key, T>>,
         template <typename, typename, typename, typename> class Observed_ =
             boost::container::flat_map,
         typename Derived = void,
         template <typename> class Signal = signal>
using flat_map = map<
    Key,
    T,
    Compare,
    Allocator,
    Observed_,
    Derived,
    Signal>;
    
}
