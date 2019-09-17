
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/object/object_view.hpp"
#include "coruja/support/macro.hpp"
#include "coruja/support/type_traits.hpp"

#include <utility>

namespace coruja { namespace detail {

namespace tag {
struct after_change{};
struct obj_for_each{};
struct for_each{};
struct before_erase{};
struct after_insert{};
} //namespace tag

struct empty_base{};

template<typename Base>
using base_or_empty = typename std::conditional<
    std::is_same<Base, void>::value,
    detail::empty_base,
    Base
>::type;
        
template<typename T>
using self_element_type = typename std::remove_pointer<T>::type;
        
template<typename Self, typename Reaction, typename To>
struct wrap_after_change_cbk : private Reaction
{
    wrap_after_change_cbk(Self& self, Reaction reaction)
        : Reaction(std::move(reaction))
        , _self(self)
    {}

    template<typename To_>
    enable_if_is_invocable_t<
        void, Reaction, self_element_type<Self>&, To_&>
    invoke_reaction(To_& o) const
    { Reaction::operator()(*_self, o); }
        
    template<typename To_>
    enable_if_is_invocable_t<
        void, Reaction, self_element_type<Self>&, const typename To_::observed_t&>
    invoke_reaction(To_& o) const
    { Reaction::operator()(*_self, o.observed()); }
    
    template<typename To_>
    typename std::enable_if<
        !(boost::hof::is_invocable<Reaction, self_element_type<Self>&, To_&>::value
          ||
          boost::hof::is_invocable<Reaction, self_element_type<Self>&,
                                   const typename To_::observed_t&>::value)
        &&
        boost::hof::is_invocable<Reaction, self_element_type<Self>&>::value,
        void>::type
    invoke_reaction(To_& o) const
    { Reaction::operator()(*_self); }
    
    template<typename To_>
    typename std::enable_if<
        !(boost::hof::is_invocable<Reaction, self_element_type<Self>&, To_&>::value
          ||
          boost::hof::is_invocable<Reaction, self_element_type<Self>&,
                                   const typename To_::observed_t&>::value)
        &&
        !boost::hof::is_invocable<Reaction, self_element_type<Self>&>::value,
        void>::type
    invoke_reaction(To_& o) const
    { Reaction::operator()(); }
        
    void operator()(To& o) const
    { invoke_reaction(o); }
    
    Self& _self;
};

template<typename Self, typename Reaction, typename To>
struct wrap_after_change_cbk<Self, Reaction, object_view<To>> :
        wrap_after_change_cbk<Self, Reaction, To>
{
    using base = wrap_after_change_cbk<Self, Reaction, To>;
    using base::base;
};

template<typename Self, typename Reaction, typename>
struct range_cbk : private Reaction
{
    range_cbk(Self& self, Reaction reaction)
        : Reaction(std::move(reaction))
        , _self(self)
    {}

    template<typename To_>
    typename std::enable_if<
        boost::hof::is_invocable<Reaction,
                                 self_element_type<Self>&,
                                 To_&,
                                 typename To_::iterator>::value
    >::type
    operator()(To_& o, typename To_::iterator it) const
    { Reaction::operator()(*_self, o, it); }

    template<typename V>
    typename std::enable_if<
        boost::hof::is_invocable<Reaction,
                                  self_element_type<Self>&,
                                  V&>::value
    >::type
    operator()(V& e) const
    { Reaction::operator()(*_self, e); }
    
    Self& _self;
};
        
}}
