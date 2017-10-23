
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/signals2/signal.hpp>

namespace coruja {

template<typename Signature>    
struct boost_signals2 : boost::signals2::signal<Signature>
{
    using base = boost::signals2::signal<Signature>;
    using base::base;
    using connection_t = boost::signals2::connection;
    using scoped_connection_t = boost::signals2::scoped_connection;
};
    
}
