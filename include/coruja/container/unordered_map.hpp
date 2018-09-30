
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/container/unordered_associative_container.hpp"
#include "coruja/container/detail/derived_or_this.hpp"
#include "coruja/support/signal.hpp"
#include "coruja/support/type_traits.hpp"

#include <boost/iterator.hpp>
#include <boost/iterator/reverse_iterator.hpp>

#include <initializer_list>		
#include <unordered_map>
#include <type_traits>

namespace coruja {

template<typename Key,
         typename T,
         typename Hash = std::hash<Key>,
         typename KeyEqual = std::equal_to<Key>,
         typename Allocator = std::allocator<std::pair<const Key, T>>,
         template <typename, typename, typename,
                   typename, typename> class Observed_ = std::unordered_map,
         typename Derived_ = void,
         template <typename> class Signal = signal>
class unordered_map : public unordered_associative_container<
    Observed_<Key, T, Hash, KeyEqual, Allocator>,
    detail::derived_or_this<
        Derived_, unordered_map<Key, T, Hash, KeyEqual, Allocator, Observed_, Derived_, Signal>>,
    Signal
    >
{
    using base = unordered_associative_container<
        Observed_<Key, T, Hash, KeyEqual, Allocator>,
    detail::derived_or_this<
        Derived_, unordered_map<Key, T, Hash, KeyEqual, Allocator, Observed_, Derived_, Signal>>,
        Signal>;

    using base::as_derived;
    using base::emit_after_insert;
    using base::emit_before_erase;
    
    using base::_container;
    
    template<typename F, template <typename> class Fwd, typename Container>
    friend typename Container::for_each_connection_t detail::for_each_by(Container&, F&&);

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
    using typename base::allocator_type;
    using typename base::const_pointer;
    using typename base::pointer;
    
    using base::begin;
    using base::cbegin;
    using base::end;
    using base::cend;
    
    using base::size;
    using base::insert;
        
    using mapped_type = typename base::observed_t::mapped_type;
    
    unordered_map() = default;
    
    unordered_map(observed_t src)
        : base(observed_t(std::move(src)))
    {}
    
    explicit unordered_map(size_type n)
        : base(observed_t(n))
    {}

    template<typename InputIterator>
    unordered_map(InputIterator first, InputIterator last)
        : base(observed_t(first, last))
    {}
    
    unordered_map(std::initializer_list<value_type> l)
        : base(observed_t(std::move(l)))
    {}
    
    unordered_map(unordered_map&&) = default;
    
    ~unordered_map()
    {
        if(!_container.empty())
            emit_before_erase(_container.begin(), _container.end());
    }
    
    unordered_map& operator=(unordered_map&&) = default;
            
    template<typename InputIt>
    void insert(InputIt first, InputIt last)
    {
        auto before_size = _container.size();
        _container.insert(first, last);
        if (_container.size() != before_size)
        {
            for(auto it(first); it != last; ++it)
            {
                auto it_ = _container.find(it->first);
                emit_after_insert(it_, std::next(it_));
            }
        }
    }
    
    void insert(std::initializer_list<value_type> ilist)
    { insert(ilist.begin(), ilist.end()); }
    
    mapped_type& at(const key_type& key)
    { return _container.at(key); }
    
    const mapped_type& at(const key_type& key) const
    { return _container.at(key); }
        
    mapped_type& operator[](const key_type& key)
    { return _container[key]; }
    
    mapped_type& operator[](key_type&& key)
    { return _container[std::move(key)]; }
    
    void rehash(size_type n)
    { return _container.rehash(n); }
};
    
}
