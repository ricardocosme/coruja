
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <vector>

namespace coruja {

class lift_signal_id_t
{
    std::vector<void*> _ids{nullptr};
public:
    lift_signal_id_t() = default;
    explicit lift_signal_id_t(std::vector<void*> ids) : _ids(std::move(ids)) {}
    const std::vector<void*>& id() const noexcept
    { return _ids; }
};

inline bool operator==(const lift_signal_id_t& lhs, const lift_signal_id_t& rhs)
{ return lhs.id() == rhs.id(); }

inline bool operator!=(const lift_signal_id_t& lhs, const lift_signal_id_t& rhs)
{ return !(lhs == rhs); }

inline bool operator<(const lift_signal_id_t& lhs, const lift_signal_id_t& rhs)
{ return lhs.id() < rhs.id(); }

inline bool operator>(const lift_signal_id_t& lhs, const lift_signal_id_t& rhs)
{ return rhs < lhs; }

inline bool operator<=(const lift_signal_id_t& lhs, const lift_signal_id_t& rhs)
{ return !(rhs < lhs); }

inline bool operator>=(const lift_signal_id_t& lhs, const lift_signal_id_t& rhs)
{ return !(lhs < rhs); }

}
