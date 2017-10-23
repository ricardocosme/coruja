
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/support/signal/connection.hpp"
#include "coruja/support/signal/scoped_connection.hpp"

#include <functional>
#include <list>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <utility>

#include <boost/mpl/vector.hpp>

namespace coruja {

template<typename Signature>
class signal;
    
template<typename R, typename... Args>
class signal<R(Args...)>
{
public:
    using args_t = boost::mpl::vector<Args...>;
    using slot_t = typename std::function<R(Args...)>;
private:
    struct slots_t : std::list<std::pair<bool, slot_t>>
    {
        using base = std::list<std::pair<bool, slot_t>>;
        using base::base;

        typename base::iterator it;
    };
    std::shared_ptr<slots_t> _slots;
    friend class any_connection;
public:
    using connection_t = connection<slots_t>;
    using scoped_connection_t = scoped_connection<connection_t>;
    
    signal()
        : _slots(new slots_t)
    {
    }

    signal(const signal&) = delete;
    signal& operator=(const signal&) = delete;
    
    signal(signal&& rhs)
        : _slots(std::move(rhs._slots))
    {
    }
    
    signal& operator=(signal&& rhs)
    {
        _slots = std::move(rhs._slots);
        return *this;
    }
    
    template<typename Slot>
    connection_t connect(Slot&& slot)
    {
        return connection_t
            (_slots,
             _slots->emplace(_slots->end(), false, std::forward<Slot>(slot)));
    }

    template<typename... PArgs>
    void operator()(PArgs&&... args)
    {
        for(_slots->it = _slots->begin(); _slots->it != _slots->end();)
        {
            auto it = (_slots->it)++;
            if(!it->first && it->second) it->second(std::forward<PArgs>(args)...);
        }
    }
};
 
}
