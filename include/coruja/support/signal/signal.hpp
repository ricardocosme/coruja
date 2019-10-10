
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/support/signal/connection.hpp"
#include "coruja/support/signal/scoped_connection.hpp"
#include "coruja/support/signal/signal_id_t.hpp"

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
    using cbk_t = typename std::function<R(Args...)>;
private:
    struct slot_t {
        bool blocked;
        cbk_t cbk;
    };
    struct slots_t : std::list<slot_t>
    {
        using base = std::list<slot_t>;
        using base::base;

        typename base::iterator it;
    };
    std::shared_ptr<slots_t> _slots;
    friend class any_connection;
public:
    using connection_t = connection<slots_t>;
    using scoped_connection_t = scoped_connection<connection_t>;
    
    signal() : _slots(new slots_t) {}

    signal(const signal&) = delete;
    signal& operator=(const signal&) = delete;
    
    signal(signal&&) = default;
    signal& operator=(signal&&) = default;
    
    template<typename Slot>
    connection_t connect(Slot&& slot)
    {
        return connection_t
            (_slots,
             _slots->insert(_slots->end(),
                            slot_t{false, std::forward<Slot>(slot)}));
    }

    template<typename... PArgs>
    void operator()(PArgs&&... args)
    {
        for(_slots->it = _slots->begin();
            _slots->it != _slots->end();)
        {
            auto it = (_slots->it)++;
            if(!it->blocked)
                it->cbk(std::forward<PArgs>(args)...);
            //TODO: Why not the following:
            // if(!_slots->it->blocked)
            //     _slots->it->cbk(std::forward<PArgs>(args)...);
            // ++_slots->it;
        }
    }

    signal_id_t id() const noexcept
    { return signal_id_t(_slots.get()); }
};
 
}
