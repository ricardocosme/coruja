
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <utility>

namespace coruja {

template<typename Connection>
class scoped_blocked_connection
{
    Connection _conn;
public:
    scoped_blocked_connection(Connection conn, bool initially_blocked = true)
        : _conn(std::move(conn))
    { if(initially_blocked) block(); }

    ~scoped_blocked_connection()
    { unblock(); }

    void block() noexcept
    { _conn.block(); }
    
    void unblock() noexcept
    { _conn.unblock(); }

    const Connection& connection() noexcept
    { return _conn; }
};
    
}

