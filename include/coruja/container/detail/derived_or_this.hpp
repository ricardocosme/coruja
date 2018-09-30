
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>

namespace coruja { namespace detail {

template<typename Derived, typename Observable>
using derived_or_this = typename std::conditional<
    std::is_same<Derived, void>::value,
    Observable,
    Derived>::type;

}}
