
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/support/type_traits.hpp"

#include <boost/fusion/include/at_c.hpp>

namespace coruja { namespace detail {

template<typename F>
struct for_each_observable {
    template<typename Obj2Conn>
    void operator()(Obj2Conn&& obj2conn) const {
        using namespace boost::fusion;
        auto& obj = at_c<0>(obj2conn);
        auto& conn = at_c<1>(obj2conn);
        
        conn = obj.for_each(f);
    }
    F f;
};

}}
