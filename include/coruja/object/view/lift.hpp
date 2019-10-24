
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/object/detail/for_each_obj2conn.hpp"
#include "coruja/object/detail/lift_to_observable.hpp"
#include "coruja/object/view/detail/build_lift_signal_id_t.hpp"
#include "coruja/object/view/detail/connect_object.hpp"
#include "coruja/object/view/object.hpp"
#include "coruja/support/connections.hpp"
#include "coruja/support/macro.hpp"

#include <boost/fusion/functional/invocation/invoke.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/zip_view.hpp>
#include <range/v3/utility/semiregular.hpp>

namespace coruja { namespace view {
        
template<typename T, typename Transform, typename... Objects>
class lift_object : view_base
{
    using From = boost::fusion::vector<Objects...>;
    
public:
    
    using signal_id_t = lift_signal_id_t;
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
        return coruja::detail::for_each_obj2conn<
            typename after_change_connection_t::type>(
                detail::connect_object
                  <From,
                   Transform,
                   remove_reference_t<F>,
                   after_change_connection_t>{_objects, _transform, f},
                _objects);
        
    }

    //Experimental: Yep, we have code duplication here but this is a
    //temp stage.
    template<typename F>
    after_change_connection_t for_each(F&& f)
    {
        f(boost::fusion::invoke(_transform, _objects));
        return after_change(std::forward<F>(f));
    }
    
    result_of_t<Transform(Objects...)> get() const noexcept
    { return boost::fusion::invoke(_transform, _objects); }

    result_of_t<Transform(Objects...)> observed() const noexcept
    { return get(); }
    
    signal_id_t after_change_id() const noexcept
    {
        std::vector<void*> ids;
        ids.reserve(boost::fusion::size(_objects));
        boost::fusion::for_each(_objects, detail::build_lift_signal_id_t{ids});
        return signal_id_t{std::move(ids)};
    }

    signal_id_t for_each_id() const noexcept
    { return after_change_id(); }
    
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
    coruja::detail::lift_to_observable_impl<remove_reference_t<Transform>>,
    decltype(view(std::declval<Objects>()))...>
    (view(std::forward<Objects>(objects))...,
     coruja::detail::lift_to_observable(std::forward<Transform>(transform)))
)

}}

#include "coruja/object/view/detail/and.hpp"
#include "coruja/object/view/detail/minus.hpp"
#include "coruja/object/view/detail/plus.hpp"
