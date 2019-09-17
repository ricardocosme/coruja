
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/object/object.hpp"
#include "coruja/object/view/object.hpp"
#include "coruja/object/view/detail/after_change_cbk.hpp"
#include "coruja/object/view/detail/transform_aux.hpp"

#include <range/v3/utility/semiregular.hpp>

namespace coruja { namespace view {
    
template<typename From, typename Transform>
class transform_object : view_base
{
public:    
    using signal_id_t = typename From::signal_id_t;
    using observed_t = result_of_t<Transform(detail::observed_t<From>)>;
    using value_type = observed_t;
    using after_change_connection_t = typename From::after_change_connection_t;

    transform_object() = default;
    
    transform_object(From from, Transform transform)
        : _transform(std::move(transform))
        , _from(std::move(from))
    {}
    
    template<typename F>
    after_change_connection_t after_change(F&& f)
    {
        return _from.after_change
            (detail::make_after_change_cbk<From>
             (_transform, std::forward<F>(f)));
    }
            
    template<typename F>
    after_change_connection_t for_each(F&& f)
    {
        return _from.for_each
            (detail::make_after_change_cbk<From>
             (_transform, std::forward<F>(f)));
    }
    
    observed_t get() const noexcept
    { return _transform(_from.get()); }
    
    observed_t observed() const noexcept
    { return get(); }
    
    //Experimental
    signal_id_t after_change_id() const noexcept
    { return _from.after_change_id(); }
    
    //Experimental
    signal_id_t for_each_id() const noexcept
    { return _from.for_each_id(); }
    
private:
    
    mutable ranges::semiregular_t<Transform> _transform;
    From _from;
};
    
template<typename ObservableObject, typename F>
inline enable_if_t<
    is_observable_object<ObservableObject>::value,
    detail::transform_object_t<ObservableObject, F>>
transform(ObservableObject&& o, F&& f)
{ return {view(std::forward<ObservableObject>(o)), std::forward<F>(f)}; }

}}

template<typename ObservableObject, typename F>
inline coruja::enable_if_t<
    coruja::is_observable_object<ObservableObject>::value,
    coruja::view::detail::transform_object_t<ObservableObject, F>>
operator|(ObservableObject&& o, F&& f)
{ return coruja::view::transform(std::forward<ObservableObject>(o), std::forward<F>(f)); }
    
