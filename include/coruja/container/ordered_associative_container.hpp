
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
class ordered_associative_container : public associative_container<Observed, Derived, Signal>
{
public:
    using base = associative_container<Observed, Derived, Signal>;
    using base::base;
        
protected:
    
    using base::_container;
    
    using base::emit_after_insert;
    using base::emit_before_erase;
    
public:

    using const_reverse_iterator = typename base::observed_t::const_reverse_iterator;
    using reverse_iterator = typename base::observed_t::reverse_iterator;
    using key_compare = typename base::observed_t::key_compare;
    
    //precondition: [first, last) must denote a sorted range of values
    //following the predicate. It's undefined behaviour when this
    //precondition isn't satisfied.
    template<typename InputIt>
    void insert_sorted(InputIt first, InputIt last)
    {
        if(first == last) return;
        
        auto beg = _container.insert(_container.end(), *first++);
        auto hint = beg;
        
        for(; first != last; ++first, ++hint)
            hint = _container.insert(hint, *first);

        emit_after_insert(beg, hint);
    }
        
    //precondition: [first, last) must denote a sorted range of values
    //following the predicate. It's undefined behaviour when this
    //precondition isn't satisfied.
    void insert_sorted(std::initializer_list<typename base::value_type> ilist)
    { insert_sorted(ilist.begin(), ilist.end()); }
    
    reverse_iterator rbegin() noexcept
    { return _container.rbegin(); }
    
    const_reverse_iterator rbegin() const noexcept
    { return const_reverse_iterator{_container.end()}; }
    
    reverse_iterator rend() noexcept
    { return _container.rend(); }
        
    const_reverse_iterator rend() const noexcept
    { return const_reverse_iterator{_container.begin()}; }
    
    const_reverse_iterator crbegin() const noexcept
    { return _container.crbegin(); }
        
    const_reverse_iterator crend() const noexcept
    { return _container.crend(); }
};
    
}
