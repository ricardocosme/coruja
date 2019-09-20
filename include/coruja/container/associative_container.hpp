
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/container/container.hpp"

namespace coruja {

template<typename Observed,
         typename Derived,
         template <typename> class Signal>    
class associative_container : public container<Observed, Derived, Signal>
{
public:
    using base = container<Observed, Derived, Signal>;
    using base::base;
        
protected:
    
    using base::_container;
    
    using base::emit_after_insert;
    using base::emit_before_erase;
    
public:

    using typename base::observed_t;
    using typename base::value_type;
    using typename base::reference;
    using typename base::const_reference;
    using typename base::iterator;
    using typename base::const_iterator;
    using typename base::difference_type;
    using typename base::size_type;

    using key_type = typename base::observed_t::key_type;
    using allocator_type = typename base::observed_t::allocator_type;
    using const_pointer = typename base::observed_t::const_pointer;
    using pointer = typename base::observed_t::pointer;
    
    using base::cbegin;
    using base::begin;
    using base::cend;
    using base::end;

    using base::erase;
    
    size_type erase(const key_type& key)
    {
        auto rng = _container.equal_range(key);
        if(rng.first == _container.end()) return 0;
        auto before_size = _container.size();
        emit_before_erase(rng.first, rng.second);
        _container.erase(rng.first, rng.second);
        auto n = before_size - _container.size();
        return n;
    }
    
    size_type count(const key_type& key) const noexcept
    { return _container.count(key); }

    template<typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args)
    {
        auto ret = _container.emplace(std::forward<Args>(args)...);
        if (ret.second)
            emit_after_insert(ret.first, std::next(ret.first));
        return ret;
    }
    
    template<typename... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args)
    {
        auto before_size = _container.size();
        auto it = _container.emplace_hint(hint, std::forward<Args>(args)...);
        if (_container.size() != before_size)
            emit_after_insert(it, std::next(it));
        return it;
    }
    
    std::pair<iterator, bool> insert(const value_type& value)
    {
        auto ret = _container.insert(value);
        if (ret.second)
            emit_after_insert(ret.first, std::next(ret.first));
        return ret;
    }
    
    std::pair<iterator, bool> insert(value_type&& value)
    {
        auto ret = _container.insert(std::move(value));
        if (ret.second)
            emit_after_insert(ret.first, std::next(ret.first));
        return ret;
    }
    
    iterator insert(const_iterator hint, const value_type& value)
    {
        auto before_size = _container.size();
        auto it = _container.insert(hint, value);
        if (_container.size() != before_size)
            emit_after_insert(it, std::next(it));
        return it;
    }
    
    iterator insert(iterator hint, value_type&& value)
    {
        auto before_size = _container.size();
        auto it = _container.insert(hint, std::move(value));
        if (_container.size() != before_size)
            emit_after_insert(it, std::next(it));
        return it;
    }
    
    template<typename InputIt>
    void insert(InputIt first, InputIt last)
    {
        for(;first != last; ++first)
            insert(_container.end(), *first);
    }

    void insert(std::initializer_list<value_type> ilist)
    { insert(ilist.begin(), ilist.end()); }
        
    const_iterator find(const key_type& key) const
    { return _container.find(key); }
    
    iterator find(const key_type& key)
    { return _container.find(key); }
};
    
}
