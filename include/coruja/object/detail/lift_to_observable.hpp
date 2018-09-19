
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/support/type_traits.hpp"
#include "coruja/support/macro.hpp"

#include <type_traits>
#include <utility>

namespace coruja { namespace detail {
    
template<typename F>    
struct lift_to_observable_impl
{
    template<typename... Objects>
    result_of_t<F(typename remove_reference_t<Objects>::observed_t...)>
    operator()(Objects&&... objects)
    { return f(objects.observed()...); }
        
    F f;
};

template<typename F>
inline auto lift_to_observable(F&& f)
CORUJA_DECLTYPE_AUTO_RETURN
( lift_to_observable_impl<remove_reference_t<F>>
  {std::forward<F>(f)} )
        
}}
