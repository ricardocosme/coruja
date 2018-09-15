
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/support/detail/connections_visitors.hpp"

#include <boost/fusion/include/all.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/vector.hpp>

namespace coruja { 

template<typename... Conns>        
struct connections
{
    using type = boost::fusion::vector<Conns...>;

    connections() = default;

    connections(type conns) : _conns(std::move(conns)) {}
    
    void disconnect()
    { boost::fusion::for_each(_conns, detail::disconnect_impl{}); }
    
    bool blocked() const noexcept
    { return boost::fusion::all(_conns, detail::is_blocked{}); }
    
    void block() noexcept
    { boost::fusion::for_each(_conns, detail::block_impl{}); }
    
    void unblock() noexcept
    { boost::fusion::for_each(_conns, detail::unblock_impl{}); }

private:
    
    type _conns;
};

}
