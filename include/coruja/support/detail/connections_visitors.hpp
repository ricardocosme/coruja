
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace coruja { namespace detail {
        
struct disconnect_impl
{
    template<typename Conn>
    void operator()(Conn&& conn) const
    { conn.disconnect(); }
};
        
struct block_impl
{
    template<typename Conn>
    void operator()(Conn&& conn) const
    { conn.block(); }
};
        
struct unblock_impl
{
    template<typename Conn>
    void operator()(Conn&& conn) const
    { conn.unblock(); }
};

struct is_blocked
{
    template<typename Conn>
    bool operator()(Conn&& conn) const
    { return conn.blocked(); }
};
        
}}
