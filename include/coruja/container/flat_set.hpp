
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/container/set.hpp"

#include <boost/container/flat_set.hpp>

namespace coruja {

template<typename Key,
         typename Compare = std::less<Key>,
         typename Allocator = std::allocator<Key>,
         template <typename, typename, typename> class Observed_ = boost::container::flat_set,
         typename Derived = void,
         template <typename> class Signal = signal>
using flat_set = set<
    Key,
    Compare,
    Allocator,
    Observed_,
    Derived,
    Signal>;
    
}
