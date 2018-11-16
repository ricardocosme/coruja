
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/object/detail/lift_to_observable.hpp"
#include "coruja/object/detail/relational_operators.hpp"
#include "coruja/object/object_base.hpp"
#include "coruja/support/signal.hpp"

#include <type_traits>
#include <utility>

namespace coruja {
    
template<class T,
         class Derived_ = void,
         template <class> class Signal = signal>  
class object : public object_base<
    T,
    typename std::conditional<
        std::is_same<Derived_, void>::value,
        object<T, Derived_, Signal>, Derived_
        >::type,
    Signal>
{
    using base = object_base<
        T,
        typename std::conditional<
            std::is_same<Derived_, void>::value,
            object, Derived_
            >::type,
        Signal>;
    
    using typename base::Derived;
    
public:
    
    using observed_t = typename base::observed_t;
    using value_type = observed_t;
    using after_change_connection_t = typename base::after_change_connection_t;

    object() = default;
    
    explicit object(observed_t observed) : _observed(std::move(observed)) {}
    
    object(object&& rhs)
        noexcept(std::is_nothrow_move_constructible<observed_t>::value)
        : base(std::move(rhs))
        , _observed(std::move(rhs._observed))
    {}
    
    object& operator=(object&& rhs)
        noexcept(std::is_nothrow_move_assignable<observed_t>::value)
    {
        base::operator=(std::move(rhs));
        _observed = std::move(rhs._observed);
        return *this;
    }
    
    template<typename U>
    enable_if_t<
        !std::is_same<remove_reference_t<U>, Derived>::value,
        Derived&>
    operator=(U&& rhs)
    {
        _observed = std::forward<U>(rhs);
        base::_after_change(base::as_derived());
        return base::as_derived();
    }
    
    const observed_t& get() const noexcept
    { return _observed; }
    
    const observed_t& observed() const noexcept
    { return get(); }
        
private:
    
    observed_t _observed;    
    
};        

}
