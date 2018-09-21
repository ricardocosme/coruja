
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/object/detail/and.hpp"
#include "coruja/object/detail/connect_object.hpp"
#include "coruja/object/detail/lift_to_observable.hpp"
#include "coruja/object/detail/minus.hpp"
#include "coruja/object/detail/plus.hpp"
#include "coruja/object/object_view.hpp"
#include "coruja/support/connections.hpp"
#include "coruja/support/macro.hpp"

#include <boost/fusion/functional/invocation/invoke.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/zip_view.hpp>
#include <range/v3/utility/semiregular.hpp>

namespace coruja { 
        
template<typename T, typename Transform, typename... Objects>
class lift_object : view_base
{
    using From = boost::fusion::vector<Objects...>;
    
public:
    
    using observed_t = T;
    using value_type = observed_t;
    using after_change_connection_t = connections<
        typename remove_reference_t<Objects>::after_change_connection_t...>;

    lift_object() = default;
                      
    lift_object(Objects... objects, Transform transform)
        : _transform(std::move(transform))
        , _objects(objects...)
    {}
    
    template<typename F>
    after_change_connection_t after_change(F&& f)
    {
        using namespace boost::fusion;

        using conns_t = typename after_change_connection_t::type;
        using Obj2Conn = vector<From&, conns_t&>;

        conns_t conns;
        
        auto obj2conn = zip_view<Obj2Conn>(Obj2Conn(_objects, conns));
        
        for_each(obj2conn, detail::connect_object
                 <From, Transform, remove_reference_t<F>, after_change_connection_t>
                 {_objects, _transform, f});
        
        return {std::move(conns)};
    }
    
    observed_t get() const noexcept
    { return boost::fusion::invoke(_transform, _objects); }

    observed_t observed() const noexcept
    { return get(); }
    
private:

    ranges::semiregular_t<Transform> _transform;
    From _objects;
};

template<typename Transform, typename... Objects>
inline auto lift(Transform&& transform, Objects&&... objects)
CORUJA_DECLTYPE_AUTO_RETURN
(
    lift_object<
        result_of_t<
            Transform(typename remove_reference_t<Objects>::observed_t...)>,
        detail::lift_to_observable_impl<remove_reference_t<Transform>>,
    decltype(view(std::declval<Objects>()))...>
    (view(std::forward<Objects>(objects))...,
     detail::lift_to_observable(std::forward<Transform>(transform)))
)

}
