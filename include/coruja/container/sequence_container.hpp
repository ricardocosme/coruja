
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
class sequence_container : public container<Observed, Derived, Signal>
{
public:
    using base = container<Observed, Derived, Signal>;
    using base::base;
    
    template<typename F>
    void assign_aux(F&& f)
    {
        if(!_container.empty())
            emit_before_erase(begin(), end());
        std::forward<F>(f)();
        emit_after_insert(begin(), end());
    }
    
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

    using base::cbegin;
    using base::begin;
    using base::cend;
    using base::end;
    using base::size;
    
    using reverse_iterator = typename base::observed_t::reverse_iterator;
    using const_reverse_iterator = typename base::observed_t::const_reverse_iterator;
    
    reverse_iterator rbegin() noexcept
    { return _container.rbegin(); }
            
    const_reverse_iterator rbegin() const noexcept
    { return const_reverse_iterator{_container.end()}; }
    
    const_reverse_iterator crbegin() const noexcept
    { return _container.crbegin(); }
    
    reverse_iterator rend() noexcept
    { return _container.rend(); }
        
    const_reverse_iterator rend() const noexcept
    { return const_reverse_iterator{_container.begin()}; }
    
    const_reverse_iterator crend() const noexcept
    { return _container.crend(); }
    
    template<typename... Args>
    iterator emplace(const_iterator pos, Args&&... args)
    {
        //GCC 4.8.2 uses iterator and not const_iterator(see const_it_cast.hpp)
        auto it = _container.emplace
            (const_it_cast(_container, pos), std::forward<Args>(args)...);
        emit_after_insert(it, std::next(it));
        return it;
    }
    
    iterator insert(const_iterator pos, const value_type& value)
    {
        //GCC 4.8.2 uses iterator and not const_iterator(see const_it_cast.hpp)
        auto it = _container.insert(const_it_cast(_container, pos), value);
        emit_after_insert(it, std::next(it));
        return it;
    }
    
    iterator insert(const_iterator pos, value_type&& value)
    {
        //GCC 4.8.2 uses iterator and not const_iterator(see const_it_cast.hpp)
        auto it = _container.insert(const_it_cast(_container, pos),
                                    std::move(value));
        emit_after_insert(it, std::next(it));
        return it;
    }
            
    //GCC 4.8.2 uses iterator and not const_iterator for the first
    //argument and `void` as return type instead of iterator
    void insert(const_iterator pos,
                size_type count,
                const value_type& value)
    {
        auto offset = std::distance(cbegin(), pos);
        //GCC 4.8.2 uses iterator and not const_iterator(see const_it_cast.hpp)
        _container.insert(const_it_cast(_container, pos), count, value);
        auto first = std::next(begin(), offset);
        emit_after_insert(first, std::next(first, count));
    }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first
    //argument and `void` as return type instead of iterator
    void insert(const_iterator pos,
                std::initializer_list<value_type> il)
    { insert(pos, il.begin(), il.end()); }
    
    //GCC 4.8.2 uses iterator and not const_iterator for the first
    //argument and `void` as return type instead of iterator
    template<typename InputIt>
    void insert(const_iterator pos, InputIt first, InputIt last)
    {
        auto offset = std::distance(cbegin(), pos);
        //GCC 4.8.2 uses iterator and not const_iterator(see const_it_cast.hpp)
        _container.insert(const_it_cast(_container, pos), first, last);
        auto count = last - first;
        auto beg = std::next(_container.begin(), offset);
        auto end = std::next(beg, count);
        emit_after_insert(beg, end);
    }
    
    void assign(size_type n, const value_type& val)
    { assign_aux([&]{_container.assign(n, val);}); }
    
    template<typename InputIterator>
    void assign(InputIterator first, InputIterator last)
    { assign_aux([&]{_container.assign(first, last);}); }

    void assign(std::initializer_list<value_type> il)
    { assign_aux([&]{_container.assign(il);}); }

    const_reference front() const noexcept
    { return _container.front(); }
    
    reference front() noexcept
    { return _container.front(); }
        
    const_reference back() const noexcept
    { return _container.back(); }
    
    reference back() noexcept
    { return _container.back(); }

    void push_back(const value_type& value)
    {
        _container.push_back(value);
        emit_after_insert(std::prev(_container.end()), _container.end());
    }
    
    void push_back(value_type&& value)
    {
        _container.push_back(std::move(value));
        emit_after_insert(std::prev(_container.end()),
                          _container.end());
    }

    //TODO: C++14 returns a reference
    template<typename...Args>
    void emplace_back(Args&&... args)
    {
        _container.emplace_back(std::forward<Args>(args)...);
        emit_after_insert(std::prev(_container.end()),
                          _container.end());
    }

    void pop_back()
    {
        emit_before_erase(std::prev(_container.rbegin().base()),
                          _container.rbegin().base());
        _container.pop_back();
    }
    void resize(size_type new_size)
    {
        auto before_size = size();
        if(new_size > before_size)
        {
            _container.resize(new_size);
            emit_after_insert(std::next(begin(), before_size), end());
        }
        else
        {
            emit_before_erase(std::next(begin(), new_size), end());
            _container.resize(new_size);
        } 
    }
    
    void resize(size_type new_size, const value_type& v)
    {
        auto before_size = size();
        if(new_size > before_size)
        {
            _container.resize(new_size, v);
            emit_after_insert(std::next(begin(), before_size), end());
        }
        else
        {
            emit_before_erase(std::next(begin(), new_size), end());
            _container.resize(new_size, v);
        } 
    }
};
    
}
