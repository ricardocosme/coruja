
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/container/sequence_container.hpp"
#include "coruja/container/detail/derived_or_this.hpp"
#include "coruja/support/signal.hpp"
#include "coruja/support/type_traits.hpp"

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
class vector : public sequence_container<
    Observed_<T, Allocator>,
    detail::derived_or_this<
        Derived_, vector<T, Allocator, Observed_, Derived_, Signal>>,
    Signal
    >
{
    using base = sequence_container<
    Observed_<T, Allocator>,
    detail::derived_or_this<
        Derived_, vector<T, Allocator, Observed_, Derived_, Signal>>,
    Signal>;
    
    using base::as_derived;
    using base::emit_after_insert;
    using base::emit_before_erase;
    
    using base::_container;
    
    template<typename F, template <typename> class Fwd, typename Container>
    friend typename Container::for_each_connection_t detail::for_each_by(Container&, F&&);

    friend struct vector_serialization;
    
public:
        
    using typename base::observed_t;
    using typename base::value_type;
    using typename base::reference;
    using typename base::const_reference;
    using typename base::iterator;
    using typename base::const_iterator;
    using typename base::difference_type;
    using typename base::size_type;
    
    using base::begin;
    using base::cbegin;
    using base::end;
    using base::cend;
    
    using base::size;
    
    using allocator_type = typename base::observed_t::allocator_type;
    using const_pointer = typename base::observed_t::const_pointer;
    using pointer = typename base::observed_t::pointer;
    
    vector()
    noexcept(std::is_nothrow_default_constructible<Allocator>::value)
        : base() {}

    explicit vector(observed_t src)
        : base(observed_t(std::move(src)))
    {}

    explicit vector(const allocator_type& alloc) noexcept
        : base(observed_t(alloc)) {}

    explicit vector(size_type n)
        : base(observed_t(n)) {}
    
    vector(size_type n, const value_type& value,
                const allocator_type& alloc = allocator_type())
        : base(observed_t(n, value, alloc)) {}
    
    vector(std::initializer_list<value_type> l,
        const allocator_type& alloc = allocator_type())
        : base(observed_t(std::move(l), alloc)) {}

    template<typename InputIterator>
    vector(InputIterator first, InputIterator last,
        const allocator_type& alloc = allocator_type())
        : base(observed_t(first, last, alloc)) {}

    vector(vector&& rhs) noexcept 
        : base(std::move(rhs))
    {}
    
    vector(vector&& rhs, const allocator_type& alloc)
        noexcept(std::allocator_traits<allocator_type>::is_always_equal::value)
        : base(std::move(rhs), alloc)
    {}
    
    ~vector()
    {
        if(!_container.empty())
            emit_before_erase(_container.begin(), _container.end());
    }
    
    vector& operator=(vector&& rhs)
        noexcept(std::is_nothrow_move_assignable<observed_t>::value)
    {
        base::operator=(std::move(rhs));
        return *this;
    }
    
    vector& operator=(std::initializer_list<value_type> il)
    {
        if(!_container.empty())
            emit_before_erase(begin(), end());
        _container = il;
        emit_after_insert(begin(), end());
        return *this;
    }
        
    void shrink_to_fit() noexcept
    { return _container.shrink_to_fit(); }
    
    size_type capacity() const noexcept
    { return _container.capacity(); }
    
    void reserve(size_type new_cap)
    { _container.reserve(new_cap); }
                        
    const_reference operator[](size_type pos) const noexcept
    { return _container[pos]; }
    
    reference operator[](size_type pos) noexcept
    { return _container[pos]; }
    
    const_reference at(size_type pos) const
    { return _container.at(pos); }    

    reference at(size_type pos)
    { return _container.at(pos); }
};

}
