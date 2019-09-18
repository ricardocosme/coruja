
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/support/macro.hpp"
#include "coruja/support/type_traits.hpp"

namespace coruja { namespace view {
    
struct And
{
    template<typename T1, typename T2>
    auto operator()(T1&& x, T2&& y) const
    CORUJA_DECLTYPE_AUTO_RETURN
    ( x && y )
};

template<typename T, typename Transform, typename... Objects>
class lift_object;

}}

template<typename O1,
         typename O2,
         typename Enable = coruja::enable_if_t<
             coruja::is_observable_object<O1>::value
             && coruja::is_observable_object<O2>::value
             >>
inline auto operator&&(O1&& o1, O2&& o2)
CORUJA_DECLTYPE_AUTO_RETURN
( coruja::view::lift(coruja::view::And{}, std::forward<O1>(o1), std::forward<O2>(o2)) )
    
