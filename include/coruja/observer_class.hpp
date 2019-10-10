
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/detail/observer_class.hpp"
#include "coruja/support/signal/any_connection.hpp"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

namespace coruja {

template<typename Derived, typename Base = void>
class observer_class : public detail::base_or_empty<Base>
{    
public:    
    class connection : public any_connection
    {
        std::vector<scoped_any_connection>* _conns;
    public:
        connection() = default;
        
        connection(std::vector<scoped_any_connection>& conns, any_connection c)
            : any_connection(std::move(c))
            , _conns(&conns)
        {}

        //precondition: the connection must be established.
        void disconnect()
        {
            _conns->erase(std::find_if
                (_conns->begin(), _conns->end(),
                 [&](scoped_any_connection& o)
                 { return o.get() == static_cast<any_connection&>(*this); }));
        }
    };
private:    
    std::unique_ptr<Derived*> _self;
    std::unique_ptr<std::vector<scoped_any_connection>>_conns;
    
    Derived& derived() noexcept
    { return static_cast<Derived&>(*this); }
   
    const Derived& derived() const noexcept
    { return static_cast<const Derived&>(*this); }

    template<typename To,
             template <typename, typename, typename> class Wrapper,
             typename Reaction>
    auto reaction_wrapper(Reaction&& reaction)
    CORUJA_DECLTYPE_AUTO_RETURN
    ( Wrapper<Derived*,
              typename std::remove_reference<Reaction>::type,
              typename std::remove_reference<To>::type>
      {*(this->_self), std::forward<Reaction>(reaction)} )
    
    template<typename To, typename Reaction>
    any_connection observe(To&& to, Reaction&& reaction, detail::tag::after_change)
    {
        return to.after_change
            (reaction_wrapper<To, detail::wrap_after_change_cbk>
             (std::forward<Reaction>(reaction)));
    }

    template<typename To, typename Reaction>
    any_connection observe(To&& to, Reaction&& reaction, detail::tag::obj_for_each)
    {
        return to.for_each
            (reaction_wrapper<To, detail::wrap_after_change_cbk>
             (std::forward<Reaction>(reaction)));
    }
    
    template<typename To, typename Reaction>
    any_connection observe(To&& to, Reaction&& reaction, detail::tag::after_insert)
    {
        return to.after_insert
            (reaction_wrapper<To, detail::range_cbk>
             (std::forward<Reaction>(reaction)));
    }

    template<typename To, typename Reaction>
    any_connection observe(To&& to, Reaction&& reaction, detail::tag::for_each)
    {
        return to.for_each
            (reaction_wrapper<To, detail::range_cbk>
             (std::forward<Reaction>(reaction)));
    }
    
    template<typename To, typename Reaction>
    any_connection observe(To&& to, Reaction&& reaction, detail::tag::before_erase)
    {
        return to.before_erase
            (reaction_wrapper<To, detail::range_cbk>
             (std::forward<Reaction>(reaction)));
    }

    template<typename Action, typename To, typename Reaction>
    connection observe_impl(To&& to, Reaction&& reaction)
    {
        _conns->emplace_back
            (observe(to, std::forward<Reaction>(reaction), Action{}));
        return {*_conns, _conns->back().get()};
    }
        
public:
    using base = detail::base_or_empty<Base>;

    observer_class()
        : base()
        , _self(new Derived*{&derived()})
        , _conns(new std::vector<scoped_any_connection>{})
    {}
    
    template<typename Arg, typename... Args,
             typename Enable = typename std::enable_if<
                 !std::is_same<typename std::remove_reference<Arg>::type,
                              Derived>::value
                 >::type>
    explicit observer_class(Arg&& arg, Args&&... args)
        : base(std::forward<Arg>(arg), std::forward<Args>(args)...)
        , _self(new Derived*{&derived()})
        , _conns(new std::vector<scoped_any_connection>{})
    {
    }
    
    template<typename To, typename Reaction>
    connection observe(To&& to, Reaction&& reaction)
    {
        return observe_impl<detail::tag::after_change>
            (to, std::forward<Reaction>(reaction));
    }
    
    //Experimental: In the best scenario, this implementation should
    //be renamed to `observe` when `object::after_change` is replaced
    //by `object::for_each`.
    template<typename To, typename Reaction>
    connection observe_obj_for_each(To&& to, Reaction&& reaction)
    {
        return observe_impl<detail::tag::obj_for_each>
            (to, std::forward<Reaction>(reaction));
    }
    
    template<typename To, typename Reaction>
    connection observe_for_each(To&& to, Reaction&& reaction)
    {
        return observe_impl<detail::tag::for_each>
            (to, std::forward<Reaction>(reaction));
    }
    
    template<typename To, typename Reaction>
    connection observe_before_erase(To&& to, Reaction&& reaction)
    {
        return observe_impl<detail::tag::before_erase>
            (to, std::forward<Reaction>(reaction));
    }
    
    template<typename To, typename Reaction>
    connection observe_after_insert(To&& to, Reaction&& reaction)
    {
        return observe_impl<detail::tag::after_insert>
            (to, std::forward<Reaction>(reaction));
    }
        
    observer_class(observer_class&& rhs) noexcept//TODO
        : base(std::move(rhs))
        , _self(std::move(rhs._self))
        , _conns(std::move(rhs._conns))
    {
        *_self = &derived();
    }

    observer_class& operator=(observer_class&& rhs) noexcept//TODO
    {
        base::operator=(std::move(rhs));
        _self = std::move(rhs._self);
        _conns = std::move(rhs._conns);
        *_self = &derived();
        return *this;
    }
};

}
