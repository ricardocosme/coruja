
// Copyright Hugo Machado 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/support/macro.hpp"

#include <algorithm>
#include <utility>

namespace coruja {

// This implementation is based on std::remove_if(from <bits/stl_algo.h>) except it doesn't
// override the elements to be erased (to preserve elements for the before_erase reaction
template<typename ForwardIterator, typename Predicate>
ForwardIterator remove_if(ForwardIterator first, ForwardIterator last, Predicate pred)
{
    first = std::find_if(first, last, pred);

    if(first == last)
        return first;

    ForwardIterator result = first;
    ++first;

    for(; first != last; ++first)
        if(!pred(*first))
        {
            auto temp = std::move(*result);
            *result = std::move(*first);
            *first = std::move(temp);
            ++result;
        }

    return result;
}

template<typename Rng, typename UnaryPredicate>
inline auto remove_if(Rng&& rng, UnaryPredicate pred)
CORUJA_DECLTYPE_AUTO_RETURN
( ::coruja::remove_if(std::begin(rng), std::end(rng), std::move(pred)) )
    
}
