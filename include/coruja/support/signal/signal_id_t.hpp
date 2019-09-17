
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace coruja {

class signal_id_t
{
    void* _id{nullptr};
public:
    signal_id_t() = default;
    explicit signal_id_t(void* id) : _id(id) {}
    void* id() const noexcept
    { return _id; }
};

inline bool operator==(const signal_id_t& lhs, const signal_id_t& rhs)
{ return lhs.id() == rhs.id(); }

inline bool operator!=(const signal_id_t& lhs, const signal_id_t& rhs)
{ return !(lhs == rhs); }

inline bool operator<(const signal_id_t& lhs, const signal_id_t& rhs)
{ return lhs.id() < rhs.id(); }

inline bool operator>(const signal_id_t& lhs, const signal_id_t& rhs)
{ return rhs < lhs; }

inline bool operator<=(const signal_id_t& lhs, const signal_id_t& rhs)
{ return !(rhs < lhs); }

inline bool operator>=(const signal_id_t& lhs, const signal_id_t& rhs)
{ return !(lhs < rhs); }

}
