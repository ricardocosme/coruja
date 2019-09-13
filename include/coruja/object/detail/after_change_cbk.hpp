
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/support/type_traits.hpp"

#include <utility>

namespace coruja { namespace detail {

template<typename From, typename Transform, typename F>    
struct after_change_cbk : protected Transform
{
    after_change_cbk() = default;
    
    after_change_cbk(Transform t, F f)
        : Transform(std::move(t))
        , _f(std::move(f))
    {}
    
    void operator()(const typename From::observed_t& from)
    { _f(Transform::operator()(from)); }

    F _f;
};

template<typename From, typename Transform, typename F>
inline after_change_cbk<From, Transform, remove_reference_t<F>>
make_after_change_cbk(Transform& transform, F&& f)
{ return {transform, std::forward<F>(f)}; }

}}
