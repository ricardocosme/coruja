
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once


#include "coruja/support/type_traits.hpp"

#include <utility>

namespace coruja { namespace detail {

template<typename F, typename Invoker>
struct invoke_observer_base : protected Invoker
{
    Invoker& as_invoker() noexcept
    { return static_cast<Invoker&>(*this); }

    const Invoker& as_invoker() const noexcept
    { return static_cast<const Invoker&>(*this); }

    invoke_observer_base(Invoker i, F f)
        : Invoker(std::move(i))
        , _f(std::move(f))
    {}

    F _f;
};

}}
