
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace coruja { namespace detail {

template <typename AssociativeContainer, typename Predicate>
inline void erase_nodes_if(AssociativeContainer& c, Predicate&& pred)
{
    for(auto it = c.begin(); it != c.end();)
        if(pred(*it)) it = c.erase(it);
        else ++it;
}

template <typename AssociativeContainer, typename U>
inline void erase(AssociativeContainer& c, const U& value)
{
    erase_nodes_if(c, [&](const typename AssociativeContainer::value_type& e)
                      { return e == value; });
}

}}
