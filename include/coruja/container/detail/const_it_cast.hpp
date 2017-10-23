
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <iterator>

namespace coruja {

/// Unfortunately this library must be compatible with GCC 4.8.2 and
/// that is a well known problem with the container's interface
/// receiving parameters as Container::iterator instead of
/// Container::const_iterator, for example `iterator erase(iterator)`
/// instead of `iterator erase(const_iterator)`. There is a clever
/// trick from Howard Hinnant to remove constness from an iterator
/// using erase(first, last), but this solution doesn't work in this
/// scenario because GCC 4.8.2 offers an `iterator erase(iterator,
/// iterator)`
template<typename Container>
inline typename Container::iterator
const_it_cast(Container& c, typename Container::const_iterator cit)
{ return std::next(c.begin(), std::distance(c.cbegin(), cit)); }
    
}
