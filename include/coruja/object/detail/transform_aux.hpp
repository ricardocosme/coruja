
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "coruja/support/type_traits.hpp"

#pragma once

namespace coruja {

template<typename From, typename Transform>
class transform_object;
    
namespace detail {
            
template<typename T>    
using view = decltype(view(std::declval<T>()));

template<typename T>    
using observed_t = typename remove_reference_t<T>::observed_t;

template<typename O1, typename F>    
using transform_object_t = transform_object<
    view<O1>, remove_reference_t<F>>;
    
}}
