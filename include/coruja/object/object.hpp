
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/support/signal.hpp"
#include "coruja/object/detail/lift_to_observable.hpp"
#include "coruja/object/detail/relational_operators.hpp"

#include <boost/hof/is_invocable.hpp>

#include <type_traits>
#include <utility>

namespace coruja {
    
template<class T,
         class Derived_ = void,
         template <class> class Signal = signal>  
class object
{
    using Derived = typename std::conditional<
        std::is_same<Derived_, void>::value,
        object, Derived_
    >::type;

    Derived& as_derived() noexcept
    { return static_cast<Derived&>(*this); }
    
    const Derived& as_derived() const noexcept
    { return static_cast<const Derived&>(*this); }
        
    using after_change_t = Signal<void(Derived&)>;
    
public:
    
    using observed_t = T;
    using value_type = observed_t;
    using after_change_connection_t = typename after_change_t::connection_t;
    
    object() = default;
    
    explicit object(observed_t observed) : _observed(std::move(observed)) {}
    
    object(object&& rhs)
        noexcept(std::is_nothrow_move_constructible<observed_t>::value)
        : _observed(std::move(rhs._observed))
        , _after_change(std::move(rhs._after_change))
    {}
    
    object& operator=(object&& rhs)
        noexcept(std::is_nothrow_move_assignable<observed_t>::value)
    {
        _observed = std::move(rhs._observed);
        _after_change = std::move(rhs._after_change);
        return *this;
    }
    
    template<typename U>
    typename std::enable_if<
        !std::is_same<typename std::remove_reference<U>::type,
                      Derived>::value,
        Derived&>::type
    operator=(U&& rhs)
    {
        _observed = std::forward<U>(rhs);
        _after_change(as_derived());
        return as_derived();
    }
    
    const observed_t& get() const noexcept
    { return _observed; }
    
    const observed_t& observed() const noexcept
    { return get(); }
    
    template<typename F>
    typename std::enable_if<
        boost::hof::is_invocable<F, Derived&>::value,
        after_change_connection_t
    >::type after_change(F&& f)
    { return _after_change.connect(std::forward<F>(f)); }
    
    template<typename F>
    typename std::enable_if<
        boost::hof::is_invocable<F, const observed_t&>::value,
        after_change_connection_t
    >::type after_change(F&& f)
    { return _after_change.connect
            (detail::lift_to_observable(std::forward<F>(f))); }

private:
    
    observed_t _observed;    
    after_change_t _after_change;
    
    //TODO: FixMe. It's a workaround to reactive_class
    template<typename F>
    after_change_connection_t r_after_change(F&& f)
    { return _after_change.connect
            (detail::lift_to_observable(std::forward<F>(f))); }
};        

}
