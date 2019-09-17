
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/support/type_traits.hpp"

#include <utility>

namespace coruja { 

struct view_base {};
    
template<typename ObservableObject>
class object_view : view_base
{
    ObservableObject* _obj{nullptr};
    
public:
    
    using observed_t = typename ObservableObject::observed_t;
    using value_type = observed_t;
    using after_change_connection_t =
        typename ObservableObject::after_change_connection_t;
    using signal_id_t = typename ObservableObject::signal_id_t;
    
    object_view() = default;
    
    object_view(ObservableObject& o) : _obj(&o) {}

    observed_t get() const noexcept
    { return _obj->observed(); }

    observed_t observed() const noexcept
    { return get(); }
    
    template<typename F>
    after_change_connection_t after_change(F&& f)
    { return _obj->after_change(std::forward<F>(f)); }
    
    //Experimental
    template<typename F>
    after_change_connection_t for_each(F&& f)
    { return _obj->for_each(std::forward<F>(f)); }
    
    //Experimental
    signal_id_t after_change_id() const noexcept
    { return _obj->after_change_id(); }

    //Experimental
    signal_id_t for_each_id() const noexcept
    { return _obj->for_each_id(); }
};
        
template<typename ObservableObject>
inline
enable_if_t<
    !std::is_base_of<
        view_base, ObservableObject>::value,    
    object_view<ObservableObject>
>
view(ObservableObject& rng)
{ return {rng}; }
    
template<typename ObservableObject>
inline
enable_if_t<
    std::is_base_of<
        view_base, remove_reference_t<ObservableObject>>::value,
    remove_reference_t<ObservableObject>
>
view(ObservableObject&& rng)
{ return std::forward<ObservableObject>(rng); }

}
