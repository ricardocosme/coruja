
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/detail/match_visitor.hpp"
#include "coruja/object/detail/lift_to_observable.hpp"
#include "coruja/object/object_base.hpp"
#include "coruja/support/signal.hpp"
#include "coruja/support/type_traits.hpp"

#include <boost/mpl/back.hpp>
#include <boost/mpl/pop_back.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/variant.hpp>

#include <utility>

namespace coruja {

struct derived_base {};
    
template<typename T>    
struct derived : derived_base {
    using type = T;
};
    
namespace serialization {
    struct variant;
}

template<typename Variant, typename TPack, typename E = void>
struct derived_or_this {
    using type = Variant;
};
    
template<typename Variant, typename TPack>
struct derived_or_this<
    Variant, TPack, typename std::enable_if<
                        std::is_base_of<derived_base,
                                        typename boost::mpl::back<TPack>::type
                                        >::value>::type>
{
    using type = typename boost::mpl::back<TPack>::type::type;
};
    
template<typename... T>
class variant : public object_base<
    typename std::conditional<
        std::is_same<
            typename derived_or_this<variant<T...>,
                                     boost::mpl::vector<T...>>::type,
            variant<T...>>::value,
        boost::variant<T...>,
        typename boost::make_variant_over<
            typename boost::mpl::pop_back<boost::mpl::vector<T...>>::type>::type
        >::type,
    typename derived_or_this<variant<T...>, boost::mpl::vector<T...>>::type,
    signal>
{
    using base = object_base<
        typename std::conditional<
            std::is_same<
                typename derived_or_this<variant<T...>,
                                         boost::mpl::vector<T...>>::type,
                variant<T...>>::value,
            boost::variant<T...>,
            typename boost::make_variant_over<
                typename boost::mpl::pop_back<boost::mpl::vector<T...>>::type
                >::type
            >::type,
        typename derived_or_this<variant<T...>, boost::mpl::vector<T...>>::type,
        signal>;
    
    using typename base::Derived;
    
public:

    using typename base::observed_t;
    
    using after_change_type_connection_t =
        typename base::after_change_t::connection_t;
    using after_change_connection_t =
        typename base::after_change_t::connection_t;
    
private:
    
    observed_t _observed;    
    typename base::after_change_t _after_change_type;

    friend serialization::variant;
    
public:
    using types = typename base::observed_t::types;

    variant() noexcept(std::is_nothrow_default_constructible<observed_t>::value)
    {}

    template<typename U>
    variant(U o)
        : _observed(std::move(o))
    {}
    
    template<typename U>
    variant& operator=(U&& o) 
    {
        auto before_type = _observed.which();
        _observed = std::forward<U>(o);
        if(before_type != _observed.which())
            _after_change_type(base::as_derived());
        base::_after_change(base::as_derived());
        return *this;
    }

    variant(variant&& rhs)
        noexcept(std::is_nothrow_move_constructible<observed_t>::value)
        : base(std::move(rhs))
        , _observed(std::move(rhs._observed))
        , _after_change_type(std::move(rhs._after_change_type))
    {
    }
    
    variant& operator=(variant&& rhs)
        noexcept(std::is_nothrow_move_assignable<observed_t>::value)
    {
        base::operator=(std::move(rhs));
        _observed = std::move(rhs._observed);
        _after_change_type = std::move(rhs._after_change_type);
        return *this;
    }
    
    int which() const noexcept
    { return _observed.which(); }
        
    bool empty() const noexcept
    { return false; }
    
    const boost::typeindex::type_info& type() const
    { return _observed.type(); }
    
    template<typename Visitor>
    typename std::decay<Visitor>::type::result_type
    visit(Visitor&& visitor)
    { return boost::apply_visitor(std::forward<Visitor>(visitor), _observed); }

    template<typename Visitor>
    typename std::decay<Visitor>::type::result_type
    visit(Visitor&& visitor) const
    { return boost::apply_visitor(std::forward<Visitor>(visitor), _observed); }
    
    template<typename Visitor>
    typename std::decay<Visitor>::type::result_type
    apply_visitor(Visitor&& visitor)
    { return visit(std::forward<Visitor>(visitor)); }

    template<typename Visitor>
    typename std::decay<Visitor>::type::result_type
    apply_visitor(Visitor&& visitor) const
    { return visit(std::forward<Visitor>(visitor)); }
    
    template<typename... Fs>
    inline void match(Fs&&... fs)
    {
        auto visitor = detail::match_visitor<typename std::decay<Fs>::type...>
            (std::forward<Fs>(fs)...);
        visit(std::move(visitor));
    }

    template<typename... Fs>
    inline void match(Fs&&... fs) const
    { const_cast<variant<T...>&>(*this).match(std::forward<Fs>(fs)...); }
        
    const observed_t& observed() const noexcept
    { return _observed; }
    
    template<typename F>
    after_change_type_connection_t after_change_type(F&& f)
    { return _after_change_type.connect(std::forward<F>(f)); }
};
    
}
