
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/container/detail/const_it_cast.hpp"
#include "coruja/support/signal.hpp"
#include "coruja/support/type_traits.hpp"

#include <initializer_list>		
#include <list>
#include <type_traits>

namespace coruja {

template<typename Signal>
struct signal_type : Signal
{ using Signal::Signal; };
    
template<typename T,
         typename Allocator = std::allocator<T>,
         template <typename, typename> class Model_ = std::list,
         typename Derived_ = void,
         template <typename> class Signal = signal>
class list
{
    using Derived = typename std::conditional<
        std::is_same<Derived_, void>::value,
        list<T, Allocator, Model_, Derived_, Signal>,
        Derived_>::type;
    
    Derived& as_derived() noexcept
    { return static_cast<Derived&>(*this); }
    
    const Derived& as_derived() const noexcept
    { return static_cast<const Derived&>(*this); }
    
    using container_t = Model_<T, Allocator>;
    container_t _container;
    
    using after_insert_rng_t = Signal<void(Derived&,
                                           typename container_t::iterator,
                                           typename container_t::iterator)>;
    
    using before_erase_rng_t = Signal<void(Derived&,
                                           typename container_t::iterator,
                                           typename container_t::iterator)>;
    
    before_erase_rng_t _before_erase;
    after_insert_rng_t _after_insert;

    template<typename F>
    struct for_each
    {
        F f;
        void operator()(Derived& derived,
                        typename container_t::iterator first,
                        typename container_t::iterator last)
        {
            for(;first != last; ++first)
                f(derived, first);
        }
    };

    template<typename F>
    void assign_aux(F&& f)
    {
        if(!_container.empty())
            _before_erase(as_derived(), begin(), end());
        std::forward<F>(f)();
        _after_insert(as_derived(), begin(), end());
    }
    
public:
    
    using before_erase_connection_t = typename before_erase_rng_t::connection_t;
    using after_insert_connection_t = typename after_insert_rng_t::connection_t;
    
    using before_erase_t = signal_type<void(Derived&,
                                       typename container_t::iterator first)>;
    
    struct after_insert_t : signal_type<void(Derived&,
                                        typename container_t::iterator first)>
    {
        using base = Signal<void(Derived&,
                                 typename container_t::iterator first)>;
        using base::base;
    };
    
    template<typename F>
    before_erase_connection_t before_erase(F&& f)
    { return _before_erase.connect(for_each<F>{std::forward<F>(f)}); }
    
    template<typename F>
    after_insert_connection_t after_insert(F&& f)
    { return _after_insert.connect(for_each<F>{std::forward<F>(f)}); }
        
public:

    using model_t = container_t;
    
    using value_type = typename container_t::value_type;
    using allocator_type = typename container_t::allocator_type;
    using size_type = typename container_t::size_type;
    using difference_type = typename container_t::difference_type;
    using reference = typename container_t::reference;    
    using const_reference = typename container_t::const_reference;
    using pointer = typename container_t::pointer;
    using const_pointer = typename container_t::const_pointer;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;
    using reverse_iterator = typename container_t::reverse_iterator;
    using const_reverse_iterator = typename container_t::const_reverse_iterator;

    list() = default;

    explicit list(const allocator_type& a)
        : _container(a)
    {}

    list(size_type n,
         const value_type& value,
         const allocator_type& a = allocator_type())
        : _container(n, value, a)
    {}

    explicit list(size_type n)
        : _container(n)
    {}

    template<typename InputIt>
    list(InputIt first, InputIt last,
         const allocator_type& a = allocator_type())
        : _container(first, last, a)
    {}

    list(std::initializer_list<T> il,
         const allocator_type& a = allocator_type())
        : _container(il, a)
    {}

    list(list&& rhs) = default;
    list& operator=(list&& rhs) = default;

    list& operator=(std::initializer_list<T> il)
    {
        assign(il.begin(), il.end());
        return *this;
    }

    iterator begin() noexcept
    { return _container.begin(); }

    reverse_iterator rbegin() noexcept
    { return _container.rbegin(); }
        
    iterator end() noexcept
    { return _container.end(); }
    
    reverse_iterator rend() noexcept
    { return _container.rend(); }
    
    const_iterator begin() const noexcept
    { return _container.begin(); }
    
    const_iterator cbegin() const noexcept
    { return _container.cbegin(); }
    
    const_reverse_iterator rbegin() const noexcept
    { return const_reverse_iterator{_container.end()}; }
    
    const_reverse_iterator crbegin() const noexcept
    { return _container.crbegin(); }
    
    const_iterator end() const noexcept
    { return _container.end(); }
    
    const_iterator cend() const noexcept
    { return _container.cend(); }
    
    const_reverse_iterator rend() const noexcept
    { return const_reverse_iterator{_container.begin()}; }
    
    const_reverse_iterator crend() const noexcept
    { return _container.crend(); }
    
    size_type size() const noexcept
    { return _container.size(); }
    
    size_type max_size() const noexcept
    { return _container.max_size(); }
    
    bool empty() const noexcept
    { return _container.empty(); }
    
    void assign(size_type n, const value_type& val)
    { assign_aux([&]{_container.assign(n, val);}); }
    
    template<typename InputIterator>
    void assign(InputIterator first, InputIterator last)
    { assign_aux([&]{_container.assign(first, last);}); }

    void assign(std::initializer_list<value_type> il)
    { assign_aux([&]{_container.assign(il);}); }
    
    template<typename... Args>
    iterator emplace(const_iterator pos, Args&&... args)
    {
        //GCC 4.8.2 uses iterator and not const_iterator(see const_it_cast.hpp)
        auto it = _container.emplace
            (const_it_cast(_container, pos), std::forward<Args>(args)...);
        _after_insert(as_derived(), it, std::next(it));
        return it;
    }
    
    void push_back(const value_type& value)
    {
        _container.push_back(value);
        _after_insert(as_derived(),
                      std::prev(_container.end()), _container.end());
    }
    
    void push_back(value_type&& value)
    {
        _container.push_back(std::move(value));
        _after_insert(as_derived(),
                      std::prev(_container.end()), _container.end());
    }
    
    template<typename...Args>
    void emplace_back(Args&&... args)
    {
        _container.emplace_back(std::forward<Args>(args)...);
        _after_insert(as_derived(), std::prev(_container.end()),
                      _container.end());
    }
    
    template<typename...Args>
    void emplace_front(Args&&... args)
    {
        _container.emplace_front(std::forward<Args>(args)...);
        _after_insert(as_derived(),
                      _container.begin(), std::next(_container.begin()));
    }
    
    void pop_back()
    {
        _before_erase(as_derived(),
                      std::prev(_container.rbegin().base()),
                      _container.rbegin().base());
        _container.pop_back();
    }

    void pop_front()
    {
        _before_erase(as_derived(),
                      _container.begin(), std::next(_container.begin()));
        _container.pop_front();
    }
    
    void push_front(const T& value)
    {
        _container.push_front(value);
        _after_insert(as_derived(),
                      _container.begin(), std::next(_container.begin()));
    }
    
    void push_front(T&& value)
    {
        _container.push_front(std::move(value));
        _after_insert(as_derived(),
                      _container.begin(), std::next(_container.begin()));
    }
    
    iterator insert(const_iterator pos, const value_type& value)
    {
        //GCC 4.8.2 uses iterator and not const_iterator(see const_it_cast.hpp)
        auto it = _container.insert(const_it_cast(_container, pos), value);
        _after_insert(as_derived(), it, std::next(it));
        return it;
    }
    
    iterator insert(const_iterator pos, value_type&& value)
    {
        //GCC 4.8.2 uses iterator and not const_iterator(see const_it_cast.hpp)
        auto it = _container.insert(const_it_cast(_container, pos),
                                    std::move(value));
        _after_insert(as_derived(),it, std::next(it));
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
        _after_insert(as_derived(),first, std::next(first, count));
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
        _after_insert(as_derived(), beg, end);
    }
    
    void resize(size_type new_size)
    {
        auto before_size = size();
        if(new_size > before_size)
        {
            _container.resize(new_size);
            _after_insert(as_derived(), std::next(begin(), before_size), end());
        }
        else
        {
            _before_erase(as_derived(), std::next(begin(), new_size), end());
            _container.resize(new_size);
        } 
    }
    
    void resize(size_type new_size, const value_type& v)
    {
        auto before_size = size();
        if(new_size > before_size)
        {
            _container.resize(new_size, v);
            _after_insert(as_derived(), std::next(begin(), before_size), end());
        }
        else
        {
            _before_erase(as_derived(), std::next(begin(), new_size), end());
            _container.resize(new_size, v);
        } 
    }
    
    const_reference front() const noexcept
    { return _container.front(); }
    
    reference front() noexcept
    { return _container.front(); }
    
    const_reference back() const noexcept
    { return _container.back(); }
    
    reference back() noexcept
    { return _container.back(); }
    
    void swap(list& other) noexcept
    { _container.swap(other._container); }
    
    void clear() noexcept
    {
        _before_erase(as_derived(), _container.begin(),
                      _container.end());
        _container.clear();
    }
    
    iterator erase(const_iterator pos)        
    {
        auto _pos = const_it_cast(_container, pos);
        _before_erase(as_derived(), _pos, std::next(_pos));
        //GCC 4.8.2 uses iterator and not const_iterator(see const_it_cast.hpp)
        auto it = _container.erase(_pos);
        return it;
    }
    
    iterator erase(const_iterator first, const_iterator last)        
    {
        auto _first = const_it_cast(_container, first);
        auto _last = const_it_cast(_container, last);
        _before_erase(as_derived(), _first, _last);
        //GCC 4.8.2 uses iterator and not const_iterator(see const_it_cast.hpp)
        auto it = _container.erase(_first, _last);
        return it;
    }        
};
    
}
