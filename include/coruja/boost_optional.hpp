
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/object/detail/lift_to_observable.hpp"
#include "coruja/object/object_base.hpp"
#include "coruja/support/signal.hpp"
#include "coruja/support/type_traits.hpp"

#include <boost/optional.hpp>

#include <utility>

namespace coruja {

namespace serialization {
    struct optional;
}
    
template<typename T,
         class Derived_ = void,
         template <typename> class Signal = signal>
class optional : public object_base<
    boost::optional<T>,
    typename std::conditional<
        std::is_same<Derived_, void>::value,
        optional<T, Derived_, Signal>, Derived_
    >::type,
    Signal>
{
    using base = object_base
        <boost::optional<T>,
         typename std::conditional<
             std::is_same<Derived_, void>::value,
             optional, Derived_
             >::type,
         Signal>;
        
    using typename base::Derived;

public:
    
    using typename base::observed_t;
    using typename base::after_change_connection_t;

private:
    
    observed_t _observed;
    
    friend serialization::optional;
    
public:

    optional() noexcept
        : _observed()
    {}

    optional(observed_t observed)
        : _observed(std::move(observed))
    {}
    
    optional(boost::none_t none) noexcept
        : _observed(none)
    {}
    
    optional(T val)
        : _observed(std::move(val))
    {}
    
    optional(bool cond, const T& val)
        : _observed(cond, val)
    {}
    
    template<typename U>
    explicit optional(optional<U>&& o)
        : _observed(std::move(o.observed()))
    {}
    
    template<typename U>
    optional& operator=(optional<U>&& o) 
    {
        _observed = std::move(o.observed());
        base::_after_change(base::as_derived());
        return *this;
    }

    optional& operator=(T val)
    {
        _observed = std::move(val);
        base::_after_change(base::as_derived());
        return *this;
    }
    
    optional& operator=(boost::none_t none) noexcept
    {
        _observed = none;
        base::_after_change(base::as_derived());
        return *this;
    }

    optional(optional&& rhs)
        noexcept(std::is_nothrow_move_constructible<observed_t>::value)
        : base(std::move(rhs))
        , _observed(std::move(rhs._observed))
    {
    }
    
    optional& operator=(optional&& rhs)
        noexcept(std::is_nothrow_move_assignable<observed_t>::value)
    {
        base::operator=(std::move(rhs));
        _observed = std::move(rhs._observed);
        return *this;
    }

    bool operator!() const noexcept
    { return !_observed; }
    
    BOOST_EXPLICIT_OPERATOR_BOOL_NOEXCEPT()
    
    const T& get() const
    { return _observed.get(); }
    
    T& get()
    { return _observed.get(); }

    const T& get_value_or(const T& v) const
    { return _observed.get_value_or(v); }
    
    T& get_value_or(T& v)
    { return _observed.get_valure_or(v); }
    
    const observed_t& observed() const noexcept
    { return _observed; }
    
};
    
//
// optional<T> vs optional<T> cases
//

template<class T>
inline bool operator==(const optional<T>& x, const optional<T>& y)
{ return x.observed() == y.observed(); }

template<class T>
inline bool operator<(const optional<T>& x, const optional<T>& y)
{ return x.observed() < y.observed(); }

template<class T>
inline bool operator!=(const optional<T>& x, const optional<T>& y)
{ return !(x == y); }

template<class T>
inline bool operator>(const optional<T>& x, const optional<T>& y)
{ return y < x; }

template<class T>
inline bool operator<=(const optional<T>& x, const optional<T>& y)
{ return !(y < x); }

template<class T>
inline bool operator>=(const optional<T>& x, const optional<T>& y)
{ return !(x < y); }
    
//
// optional<T> vs T cases
//
template<class T>
inline
bool operator==(const optional<T>& x, const T& y)
{ return x.observed() == y; }

template<class T>
inline
bool operator<(const optional<T>& x, const T& y)
{ return x.observed() < y; }

template<class T>
inline
bool operator!=(const optional<T>& x, const T& y)
{ return !(x == y); }

template<class T>
inline
bool operator>(const optional<T>& x, const T& y)
{ return y < x; }

template<class T>
inline
bool operator<=(const optional<T>& x, const T& y)
{ return !(y < x); }

template<class T>
inline
bool operator>=(const optional<T>& x, const T& y)
{ return !(x < y); }
    
//
//  T cases vs optional<T>
//
template<class T>
inline bool operator==(const T& x, const optional<T>& y)
{ return x == y.observed(); }

template<class T>
inline bool operator<(const T& x, const optional<T>& y)
{ return x < y.observed(); }

template<class T>
inline bool operator!=(const T& x, const optional<T>& y)
{ return !(x == y); }

template<class T>
inline bool operator>(const T& x, const optional<T>& y)
{ return y < x; }

template<class T>
inline bool operator<=(const T& x, const optional<T>& y)
{ return !(y < x); }

template<class T>
inline bool operator>=(const T& x, const optional<T>& y)
{ return !(x < y); }
    
}
