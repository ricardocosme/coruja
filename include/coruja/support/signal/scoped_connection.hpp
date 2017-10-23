
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <utility>

namespace coruja {

template<typename Connection>
class scoped_connection;
    
template<typename Connection>
inline bool operator<(const scoped_connection<Connection>& lhs,
                      const scoped_connection<Connection>& rhs);
    
template<typename Connection>
class scoped_connection
{
    Connection _conn;
    
    friend bool operator< <>(const scoped_connection&,
                             const scoped_connection&);
    
public:
    scoped_connection() = default;
    
    scoped_connection(Connection conn)
        : _conn(std::move(conn))
    {}
    
    scoped_connection(const scoped_connection&) = delete;
    scoped_connection& operator=(const scoped_connection&) = delete;
    
    scoped_connection(scoped_connection&& rhs) noexcept
    {
        _conn = std::move(rhs._conn);
        rhs._conn = Connection();
    }

    scoped_connection& operator=(scoped_connection&& rhs) noexcept
    {
        if(_conn != Connection()) _conn.disconnect();
        _conn = std::move(rhs._conn);
        rhs._conn = Connection();
        return *this;
    }

    ~scoped_connection()
    { if(_conn != Connection()) _conn.disconnect(); }

    Connection& get() noexcept
    { return _conn; }
    
    const Connection& get() const noexcept
    { return _conn; }
    
    Connection release() noexcept
    {
        Connection ret(std::move(_conn));
        _conn = Connection();
        return ret;
    }
};
    
template<typename Connection>
inline bool operator==(const scoped_connection<Connection>& lhs,
                       const scoped_connection<Connection>& rhs)
{ return lhs._conn == rhs._conn; }
    
template<typename Connection>
inline bool operator!=(const scoped_connection<Connection>& lhs,
                       const scoped_connection<Connection>& rhs)
{ return !(lhs == rhs); }

template<typename Connection>
inline bool operator<(const scoped_connection<Connection>& lhs,
                      const scoped_connection<Connection>& rhs)
{ return lhs._conn < rhs._conn; }
    
template<typename Connection>
inline bool operator>(const scoped_connection<Connection>& lhs,
                      const scoped_connection<Connection>& rhs)
{ return rhs._conn < lhs._conn; }
    
template<typename Connection>
inline bool operator<=(const scoped_connection<Connection>& lhs,
                       const scoped_connection<Connection>& rhs)
{ return !(rhs._conn < lhs._conn); }
    
template<typename Connection>
inline bool operator>=(const scoped_connection<Connection>& lhs,
                       const scoped_connection<Connection>& rhs)
{ return !(lhs._conn < rhs._conn); }
    
}

