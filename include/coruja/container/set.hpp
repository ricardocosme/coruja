
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/container/ordered_associative_container.hpp"
#include "coruja/container/detail/derived_or_this.hpp"
#include "coruja/support/signal.hpp"
#include "coruja/support/type_traits.hpp"

#include <boost/iterator.hpp>
#include <boost/iterator/reverse_iterator.hpp>

#include <initializer_list>		
#include <set>
#include <type_traits>

namespace coruja {

template<typename Key,
         typename Compare = std::less<Key>,
         typename Allocator = std::allocator<Key>,
         template <typename, typename, typename> class Observed_ = std::set,
         typename Derived_ = void,
         template <typename> class Signal = signal>
class set : public ordered_associative_container<
    Observed_<Key, Compare, Allocator>,
    detail::derived_or_this<
        Derived_, set<Key, Compare, Allocator, Observed_, Derived_, Signal>>,
    Signal
    >
{
    using base = ordered_associative_container<
    Observed_<Key, Compare, Allocator>,
    detail::derived_or_this<
        Derived_, set<Key, Compare, Allocator, Observed_, Derived_, Signal>>,
        Signal>;
        
    using base::as_derived;
    using base::emit_after_insert;
    using base::emit_before_erase;
    
    using base::_container;
    
    friend struct map_serialization;
    
public:

    using typename base::observed_t;
    using typename base::value_type;
    using typename base::reference;
    using typename base::const_reference;
    using typename base::iterator;
    using typename base::const_iterator;
    using typename base::difference_type;
    using typename base::size_type;
    using typename base::key_type;
    using typename base::key_compare;
    using typename base::const_reverse_iterator;
    using typename base::allocator_type;
    using typename base::reverse_iterator;
    using typename base::const_pointer;
    using typename base::pointer;
    
    using base::begin;
    using base::cbegin;
    using base::end;
    using base::cend;
    
    using base::size;
    using base::insert;
    
    using value_compare = typename base::observed_t::value_compare;
    
    set() = default;

    set(observed_t src)
        : base(observed_t(src.begin(), src.end()))
    {}
    
    template<typename InputIterator>
    set(InputIterator first, InputIterator last)
        : base(observed_t(first, last))
    {}
    
    set(std::initializer_list<value_type> l)
        : base(observed_t(std::move(l)))
    {}
    
    set(set&&) = default;

    ~set()
    {
        if(!_container.empty())
            emit_before_erase(_container.begin(), _container.end());
    }
    
    set& operator=(set&&) = default;
    
    std::pair<iterator, iterator> equal_range
    (const key_type& key)
    { return _container.equal_range(key); }
    
    std::pair<const_iterator,
              const_iterator> equal_range
    (const key_type& key) const
    { return _container.equal_range(key); }
    
    iterator lower_bound(const key_type& key)
    { return _container.lower_bound(key); }
    
    const_iterator lower_bound(const key_type& key) const
    { return _container.lower_bound(key); }
        
    key_compare key_comp() const
    { return _container.key_comp(); }
    
    value_compare value_comp() const
    { return key_comp(); }
    
    allocator_type get_allocator() const noexcept
    { return _container.get_allocator(); }
};

template<typename Key,
         typename Compare,
         typename Allocator,
         template <typename, typename, typename> class Model,
         typename Derived,
         template <typename> class Signal = signal>
bool operator==(const set<Key, Compare, Allocator, Model, Derived, Signal>& lhs,
                const set<Key, Compare, Allocator, Model, Derived, Signal>& rhs)
{
    return lhs.observed() == rhs.observed();
}
    
template<typename Key,
         typename Compare,
         typename Allocator,
         template <typename, typename, typename> class Model,
         typename Derived,
         template <typename> class Signal = signal>
bool operator!=(const set<Key, Compare, Allocator, Model, Derived, Signal>& lhs,
                const set<Key, Compare, Allocator, Model, Derived, Signal>& rhs)
{
    return !(lhs == rhs);
}

}
