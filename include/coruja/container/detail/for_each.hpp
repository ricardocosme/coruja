
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/support/type_traits.hpp"

#include <utility>

namespace coruja { namespace detail {
            
template<typename F, typename Fwd>
struct for_each_impl
{
    template<typename Derived, typename It>
    void operator()(Derived& derived, It first, It last)
    {
        for(;first != last; ++first)
            Fwd{f}(derived, first);
    }
    remove_reference_t<F> f;
};

template<typename F>
struct fwd_by_it
{
    template<typename Derived, typename It>
    void operator()(Derived& derived, It it) 
    { f(derived, it); }
    F& f;
};

template<typename F>
struct fwd_by_ref
{
    template<typename Derived, typename It>
    void operator()(Derived&, It it) 
    { f(*it); }
    F& f;
};

template<typename F, template <typename> class Fwd, typename Container>
inline typename Container::for_each_connection_t
for_each_by(Container& c, F&& f)
{
    using F_d = typename std::decay<F>::type;
    for_each_impl<F_d, Fwd<F_d>>
        cbk{std::forward<F>(f)};
    cbk(c, c.begin(), c.end());
    return c._after_insert.connect(std::move(cbk));
}
            
}}
