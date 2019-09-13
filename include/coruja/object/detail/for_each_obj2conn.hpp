
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/zip_view.hpp>

#include <utility>

namespace coruja { namespace detail {

template<typename Conns, typename Visitor, typename ObservableObjects>
inline Conns for_each_obj2conn(Visitor&& visitor, ObservableObjects&& objs)
{
    namespace fs = boost::fusion;
    
    using obj2conn_t = fs::vector<ObservableObjects&, Conns&>;

    Conns conns;
    
    auto obj2conn = fs::zip_view<obj2conn_t>(obj2conn_t(objs, conns));
    fs::for_each(obj2conn, visitor);
    
    return {std::move(conns)};
}

}}
