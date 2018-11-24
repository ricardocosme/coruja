
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/hof/is_invocable.hpp>

namespace coruja {

template<typename Observed,
         typename Derived_,
         template <class> class Signal>  
class object_base
{
protected:

    using Derived = Derived_;
    
    Derived& as_derived() noexcept
    { return static_cast<Derived&>(*this); }
    
    const Derived& as_derived() const noexcept
    { return static_cast<const Derived&>(*this); }
    
    using after_change_t = Signal<void(Derived&)>;
    
    after_change_t _after_change;
    
public:

    using observed_t = Observed;
    using after_change_connection_t = typename after_change_t::connection_t;
    
    template<typename F>
    enable_if_is_invocable_t<after_change_connection_t, F, Derived&>
    after_change(F&& f)
    { return _after_change.connect(std::forward<F>(f)); }
    
    template<typename F>
    enable_if_is_invocable_t<after_change_connection_t, F, const observed_t&>
    after_change(F&& f)
    { return _after_change.connect
            (detail::lift_to_observable(std::forward<F>(f))); }

    //Experimental
    template<typename F>
    enable_if_is_invocable_t<after_change_connection_t, F, Derived&>
    for_each(F&& f)
    {
        f(as_derived());
        return after_change(std::forward<F>(f));
    }
    
    //Experimental
    template<typename F>
    enable_if_is_invocable_t<after_change_connection_t, F, const observed_t&>
    for_each(F&& f)
    {
        detail::lift_to_observable_impl<F&>{f}(as_derived());
        return after_change(std::forward<F>(f));
    }
};
    
}
