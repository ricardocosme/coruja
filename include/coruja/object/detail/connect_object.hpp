
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/fusion/functional/invocation/invoke.hpp>

namespace coruja { namespace detail {

template<typename Objects, typename Transform, typename F, typename Observed>
struct lift_f : private Transform {
    lift_f(Transform transform, Objects objects, F f)
        : Transform(std::move(transform))
        , _objects(std::move(objects))
        , _f(std::move(f))
    {}
    
    void operator()(const Observed&)
    { _f(boost::fusion::invoke(static_cast<Transform&>(*this), _objects)); }
    
    Objects _objects;
    F _f;
};
        
template<typename From, typename Transform, typename F, typename Conn>
struct connect_object
{
    template<typename Obj2Conn>
    void operator()(Obj2Conn&& obj2conn) const
    {
        using namespace boost::fusion;
        auto& object = at_c<0>(obj2conn);
        auto& conn = at_c<1>(obj2conn);

        using Obj = typename std::remove_reference<
            typename result_of::at_c<Obj2Conn, 0>::type>::type;
        
        //TODO: Update from
        conn = object.after_change
            (lift_f<From, Transform, F, typename Obj::observed_t>
             (_transform, _from, _f));
    }
    
    From& _from;
    Transform& _transform;
    F& _f;
};

}}
