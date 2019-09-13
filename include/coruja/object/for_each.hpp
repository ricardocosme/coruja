
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/object/detail/for_each_observable.hpp"
#include "coruja/object/detail/for_each_obj2conn.hpp"
#include "coruja/support/connections.hpp"
#include "coruja/support/type_traits.hpp"

#include <boost/fusion/include/vector.hpp>

namespace coruja {

template<typename F,
         typename... ObservableObjects,
         typename connections_t = connections<
             typename remove_reference_t<ObservableObjects>::after_change_connection_t...>
         >
inline connections_t for_each(F&& f, ObservableObjects&&... pobjs)
{
    return detail::for_each_obj2conn<typename connections_t::type>(
        detail::for_each_observable<F>{std::forward<F>(f)},
        boost::fusion::vector<ObservableObjects...>(
            std::forward<ObservableObjects>(pobjs)...));
}

}
