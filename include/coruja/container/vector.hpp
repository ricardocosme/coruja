
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/container/detail/const_it_cast.hpp"
#include "coruja/container/detail/for_each.hpp"
#include "coruja/support/signal.hpp"
#include "coruja/support/type_traits.hpp"

#include <boost/hof/is_invocable.hpp>
#include <initializer_list>		
#include <type_traits>
#include <vector>

namespace coruja { 

struct vector_serialization;
    
template<typename T,
         typename Allocator = std::allocator<T>,
         template <typename, typename> class Observed_ = std::vector,
         typename Derived_ = void,
         template <typename> class Signal = signal>
class vector
{
    using Derived = typename std::conditional<
        std::is_same<Derived_, void>::value,
        vector, Derived_
    >::type;

    Derived& as_derived() noexcept
    { return static_cast<Derived&>(*this); }
    
    const Derived& as_derived() const noexcept
    { return static_cast<const Derived&>(*this); }
    
    using container_t = Observed_<T, Allocator>;
    
    using after_insert_rng_t = Signal<void(Derived&,
                                           typename container_t::iterator,
                                           typename container_t::iterator)>;
    
    using before_erase_rng_t = Signal<void(Derived&,
                                           typename container_t::iterator,
                                           typename container_t::iterator)>;
    
    container_t _container;
    
    before_erase_rng_t _before_erase;
    after_insert_rng_t _after_insert;
    
    void emit_after_insert(typename container_t::iterator first,
                           typename container_t::iterator last)
    { _after_insert(as_derived(), first, last); }
    
    void emit_before_erase(typename container_t::iterator first,
                           typename container_t::iterator last)
    { _before_erase(as_derived(), first, last); }
    
    template<typename F, template <typename> class Fwd, typename Container>
    friend typename Container::for_each_connection_t detail::for_each_by(Container&, F&&);

    friend struct vector_serialization;
    
public:
        
    using observed_t = container_t;
    
    using for_each_connection_t = typename after_insert_rng_t::connection_t;
    using before_erase_connection_t = typename before_erase_rng_t::connection_t;

    //Deprecated
    using after_insert_connection_t = typename after_insert_rng_t::connection_t;
    
    using size_type = typename container_t::size_type;
    using value_type = typename container_t::value_type;
    using const_iterator = typename container_t::const_iterator;
    using const_reverse_iterator = typename container_t::const_reverse_iterator;
    using const_reference = typename container_t::const_reference;
    using allocator_type = typename container_t::allocator_type;
    using iterator = typename container_t::iterator;
    using reverse_iterator = typename container_t::reverse_iterator;
    using const_pointer = typename container_t::const_pointer;
    using difference_type = typename container_t::difference_type;
    using reference = typename container_t::reference;    
    using pointer = typename container_t::pointer;
    
    vector()
    noexcept(std::is_nothrow_default_constructible<Allocator>::value)
        : _container() {}

    explicit vector(observed_t src)
        : _container(std::move(src))
    {}

    explicit vector(const allocator_type& alloc) noexcept
        : _container(alloc) {}

    explicit vector(size_type n)
        : _container(n) {}
    
    vector(size_type n, const value_type& value,
                const allocator_type& alloc = allocator_type())
        : _container(n, value, alloc) {}
    
    vector(std::initializer_list<value_type> l,
        const allocator_type& alloc = allocator_type())
        : _container(std::move(l), alloc) {}

    template<typename InputIterator>
    vector(InputIterator first, InputIterator last,
        const allocator_type& alloc = allocator_type())
        : _container(first, last, alloc) {}

    vector(vector&& rhs) noexcept 
        : _container(std::move(rhs._container))
        , _before_erase(std::move(rhs._before_erase))
        , _after_insert(std::move(rhs._after_insert))
    {}
    
    vector& operator=(vector&&) = default;
    
    vector& operator=(std::initializer_list<value_type> il)
    {
        if(!_container.empty())
            emit_before_erase(begin(), end());
        _container = std::move(il);
        emit_after_insert(begin(), end());
        return *this;
    }
    
    vector(vector&& rhs, const allocator_type& alloc)
        noexcept(std::allocator_traits<allocator_type>::is_always_equal::value)
        : _container(std::move(rhs._container), alloc)
        , _before_erase(std::move(rhs._before_erase))
        , _after_insert(std::move(rhs._after_insert))
    {}
    
    ~vector()
    {
        if(!_container.empty())
            emit_before_erase(_container.begin(), _container.end());
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
    
    void resize(size_type new_size)
    {
        if(new_size < size())
        {
            emit_before_erase(begin() + new_size, end());
            _container.resize(new_size);
            return;
        }
        _container.resize(new_size);
    }
    
    void resize(size_type new_size, const value_type& v)
    {
        if(new_size < size())
        {
            emit_before_erase(begin() + new_size, end());
            _container.resize(new_size, v);
        }
        else if(new_size > size())
        {
            auto before_size = size();
            _container.resize(new_size, v);
            _after_insert(as_derived(), std::next(begin(), before_size),
                          end());
        }
    }
    
    void shrink_to_fit() noexcept
    { return _container.shrink_to_fit(); }
    
    size_type capacity() const noexcept
    { return _container.capacity(); }
    
    bool empty() const noexcept
    { return _container.empty(); }    
    
    void reserve(size_type new_cap)
    { _container.reserve(new_cap); }
                    
    void clear() noexcept
    {
        emit_before_erase(_container.begin(), _container.end());
        _container.clear();
    }
    
    void swap(vector& other) noexcept
    {
        if(!_container.empty())
            emit_before_erase(begin(), end());
        if(!other._container.empty())
            other._before_erase(other.as_derived(),
                                other.begin(), other.end());
        _container.swap(other._container);
        emit_after_insert(begin(), end());
        other.emit_after_insert(other.begin(), other.end());
    }
    
    void assign(size_type n, const value_type& val)
    {
        if(!_container.empty())
            emit_before_erase(begin(), end());
        _container.assign(n, val);        
        emit_after_insert(begin(), end());
    }

    template<typename InputIterator>
    void assign(InputIterator first, InputIterator last)
    {
        if(!_container.empty())
            emit_before_erase(begin(), end());
        _container.assign(first, last);
        emit_after_insert(begin(), end());
    }
    
    void assign(std::initializer_list<value_type> il)
    {
        if(!_container.empty())
            emit_before_erase(begin(), end());
        _container.assign(std::move(il));
        emit_after_insert(begin(), end());
    }
            
    const_reference operator[](size_type pos) const noexcept
    { return _container[pos]; }
    
    reference operator[](size_type pos) noexcept
    { return _container[pos]; }
    
    const_reference at(size_type pos) const
    { return _container.at(pos); }    

    reference at(size_type pos)
    { return _container.at(pos); }
    
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
        auto offset = pos - begin();
        //GCC 4.8.2 uses iterator and not const_iterator(see const_it_cast.hpp)
        _container.insert(const_it_cast(_container, pos), count, value);
        auto first = begin() + offset;
        emit_after_insert(first, first + count);
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
        auto offset = pos - begin();
        //GCC 4.8.2 uses iterator and not const_iterator(see const_it_cast.hpp)
        _container.insert(const_it_cast(_container, pos), first, last);
        auto count = last - first;
        auto beg = _container.begin() + offset;
        auto end = beg + count;
        emit_after_insert(beg, end);
    }
        
    iterator erase(const_iterator pos)        
    {
        auto _pos = const_it_cast(_container, pos);
        emit_before_erase(_pos, std::next(_pos));
        //GCC 4.8.2 uses iterator and not const_iterator(see const_it_cast.hpp)
        auto it = _container.erase(_pos);
        return it;
    }
    
    iterator erase(const_iterator first, const_iterator last)        
    {
        auto _first = const_it_cast(_container, first);
        auto _last = const_it_cast(_container, last);
        emit_before_erase(_first, _last);
        //GCC 4.8.2 uses iterator and not const_iterator(see const_it_cast.hpp)
        auto it = _container.erase(_first, _last);
        return it;
    }
               
    template<typename F>
    typename std::enable_if<
        !boost::hof::is_invocable<
            F, reference>::value,
        for_each_connection_t
    >::type
    for_each(F&& f)
    { return detail::for_each_by<F, detail::fwd_by_it>(as_derived(), std::forward<F>(f)); }
    
    template<typename F>
    typename std::enable_if<
        boost::hof::is_invocable<
            F, reference>::value,
        for_each_connection_t
    >::type
    for_each(F&& f)
    { return detail::for_each_by<F, detail::fwd_by_ref>(as_derived(), std::forward<F>(f)); }

    const observed_t& observed() const noexcept
    { return _container; }

    template<typename F>
    typename std::enable_if<
        !boost::hof::is_invocable<
            F, reference>::value,
        before_erase_connection_t
    >::type
    before_erase(F&& f)
    {
        return _before_erase.connect
            (detail::for_each_impl<F, detail::fwd_by_it<F&>>{std::forward<F>(f)});
    }    
    
    template<typename F>
    typename std::enable_if<
        boost::hof::is_invocable<
            F, reference>::value,
        before_erase_connection_t
    >::type
    before_erase(F&& f)
    {
        return _before_erase.connect
            (detail::for_each_impl<F, detail::fwd_by_ref<F&>>{std::forward<F>(f)});
    }
    
    //Deprecated
    template<typename F>
    typename std::enable_if<
        !boost::hof::is_invocable<
            F, reference>::value,
        after_insert_connection_t
    >::type
    after_insert(F&& f)
    {
        return _after_insert.connect
            (detail::for_each_impl<F, detail::fwd_by_it<F&>>{std::forward<F>(f)});
    }    
    
    //Deprecated
    template<typename F>
    typename std::enable_if<
        boost::hof::is_invocable<
            F, reference>::value,
        after_insert_connection_t
    >::type
    after_insert(F&& f)
    {
        return _after_insert.connect
            (detail::for_each_impl<F, detail::fwd_by_ref<F&>>{std::forward<F>(f)});
    }
};
    
}
