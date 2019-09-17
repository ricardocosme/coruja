
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/support/signal/signal_id_t.hpp"
#include "coruja/support/signal/lift_signal_id_t.hpp"

#include <boost/variant/variant.hpp>

namespace coruja {

using any_signal_id_t = boost::variant<signal_id_t, lift_signal_id_t>;

}
