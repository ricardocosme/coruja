
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
struct lift_to_observable_impl : private F
{
    lift_to_observable_impl() = default;
    
    lift_to_observable_impl(F f) : F(std::move(f)) {}
    
    template<typename... Objects>
    auto operator()(Objects&&... objects)
    CORUJA_DECLTYPE_AUTO_RETURN
    ( F::operator()(objects.get()...) )
};

template<typename F>
inline auto lift_to_observable(F&& f)
CORUJA_DECLTYPE_AUTO_RETURN
( lift_to_observable_impl<remove_reference_t<F>>
  {std::forward<F>(f)} )
        
}}
