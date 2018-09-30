
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/container/associative_container.hpp"

namespace coruja {

template<typename Observed,
         typename Derived,
         template <typename> class Signal>    
class unordered_associative_container : public associative_container<Observed, Derived, Signal>
{
public:
    using base = associative_container<Observed, Derived, Signal>;
    using base::base;
        
protected:
    
    using base::_container;
    
    using base::emit_after_insert;
    using base::emit_before_erase;
};
    
}
