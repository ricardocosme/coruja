
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <memory>
#include <utility>

namespace coruja {

template<typename>
class scoped_blocked_connection;
    
template<typename Slots>    
class connection;
    
template<typename Slots>
inline bool operator==(const connection<Slots>& lhs,
                       const connection<Slots>& rhs);
    
template<typename Slots>
inline bool operator<(const connection<Slots>& lhs,
                      const connection<Slots>& rhs);
    
template<typename Slots>    
class connection
{
    std::weak_ptr<Slots> _slots;
    typename Slots::iterator _it;

    template<typename>
    friend class scoped_blocked_connection;

    friend bool operator==<>(const connection&,
                             const connection&);
    
    friend bool operator< <>(const connection&,
                             const connection&);
public:
    connection() = default;
    
    explicit connection(std::weak_ptr<Slots> pslots, typename Slots::iterator it)
        : _slots(pslots)
        , _it(it)
    {}

    connection(const connection&) = default;
    connection& operator=(const connection&) = default;

    connection(connection&& rhs) noexcept
        : _slots(std::move(rhs._slots))
        , _it(std::move(rhs._it))
    {
        //C++11 doesn't have weak_ptr(weak_ptr&&)
        rhs._slots.reset();
    }
        
    connection& operator=(connection&& rhs) noexcept
    {
        _slots = std::move(rhs._slots);
        _it = std::move(rhs._it);
        //C++11 doesn't have weak_ptr(weak_ptr&&)
        rhs._slots.reset();
        return *this;
    }
    
    void disconnect()
    {
        auto pslots = _slots.lock();
        if(pslots) pslots->it = pslots->erase(_it);
    }

    bool blocked() const noexcept
    { return _it->blocked; }
    
    void block() noexcept
    { _it->blocked = true; }
    
    void unblock() noexcept
    { _it->blocked = false; }
};

template<typename Slots>
inline bool operator==(const connection<Slots>& lhs,
                       const connection<Slots>& rhs)
{ return lhs._slots.lock() == rhs._slots.lock() && lhs._it == rhs._it; }
    
template<typename Slots>
inline bool operator!=(const connection<Slots>& lhs,
                       const connection<Slots>& rhs)
{ return !(lhs == rhs); }

template<typename Slots>
inline bool operator<(const connection<Slots>& lhs,
                      const connection<Slots>& rhs)
{ return lhs._slots.lock() < rhs._slots.lock(); }
    
template<typename Slots>
inline bool operator>(const connection<Slots>& lhs,
                      const connection<Slots>& rhs)
{ return rhs._slots.lock() < lhs._slots.lock(); }
    
template<typename Slots>
inline bool operator<=(const connection<Slots>& lhs,
                       const connection<Slots>& rhs)
{ return !(rhs._slots.lock() < lhs._slots.lock()); }
    
template<typename Slots>
inline bool operator>=(const connection<Slots>& lhs,
                       const connection<Slots>& rhs)
{ return !(lhs._slots.lock() < rhs._slots.lock()); }
            
}

