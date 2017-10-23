
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/object/object.hpp"
#include "coruja/object/object_view.hpp"
#include "coruja/object/detail/after_change_cbk.hpp"

#include <range/v3/utility/semiregular.hpp>

namespace coruja { 
    
template<typename From, typename T, typename Transform>
class transform_object : view_base
{
public:    
    using observed_t = T;
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
            (detail::after_change_cbk<From, Transform, F>
             {_transform, std::forward<F>(f)});
    }
            
    observed_t get() const noexcept
    { return _transform(_from.get()); }
    
    observed_t observed() const noexcept
    { return get(); }
    
private:
    
    mutable ranges::semiregular_t<Transform> _transform;
    From _from;
};
    
template<typename ObservableObject,
         typename F,
         typename T2 = typename std::result_of<
             F(typename std::remove_reference<ObservableObject>::type::observed_t)>::type,
         typename Ret = transform_object<
             decltype(view(std::declval<ObservableObject>())),
             T2,
             typename std::remove_reference<F>::type>>
inline typename std::enable_if<
    is_observable_object<
             typename std::remove_reference<ObservableObject>::type>::value,
    Ret>::type
transform(ObservableObject&& o, F&& f)
{ return {view(std::forward<ObservableObject>(o)), std::forward<F>(f)}; }
    
}
