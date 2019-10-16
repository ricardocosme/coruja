
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/container/sequence_container.hpp"
#include "coruja/container/detail/derived_or_this.hpp"
#include "coruja/support/signal.hpp"
#include "coruja/support/type_traits.hpp"

#include <initializer_list>		
#include <list>
#include <type_traits>

namespace coruja {

template<typename T,
         typename Allocator = std::allocator<T>,
         template <typename, typename> class Observed_ = std::list,
         typename Derived_ = void,
         template <typename> class Signal = signal>
class list : public sequence_container<
    Observed_<T, Allocator>,
    detail::derived_or_this<
        Derived_, list<T, Allocator, Observed_, Derived_, Signal>>,
    Signal
    >
{
    using base = sequence_container<
        Observed_<T, Allocator>,
        detail::derived_or_this<
            Derived_, list<T, Allocator, Observed_, Derived_, Signal>>,
        Signal>;
    
    using base::emit_after_insert;
    using base::emit_before_erase;
    
    using base::_container;
    
    friend struct sequence_serialization;
    
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

    using base::erase;
    using base::size;
    using base::assign;
    
    using allocator_type = typename base::observed_t::allocator_type;
    using pointer = typename base::observed_t::pointer;
    using const_pointer = typename base::observed_t::const_pointer;

    list() = default;

    explicit list(const allocator_type& a)
        : base(observed_t(a))
    {}

    list(size_type n,
         const value_type& value,
         const allocator_type& a = allocator_type())
        : base(observed_t(n, value, a))
    {}

    explicit list(size_type n)
        : base(observed_t(n))
    {}

    template<typename InputIt>
    list(InputIt first, InputIt last,
         const allocator_type& a = allocator_type())
        : base(observed_t(first, last, a))
    {}

    list(std::initializer_list<T> il,
         const allocator_type& a = allocator_type())
        : base(observed_t(il, a))
    {}

    list(list&& rhs) = default;
    list& operator=(list&& rhs) = default;

    list& operator=(std::initializer_list<T> il)
    {
        assign(il.begin(), il.end());
        return *this;
    }

    ~list()
    {
        if(!_container.empty())
            emit_before_erase(_container.begin(), _container.end());
    }
                            
    template<typename...Args>
    void emplace_front(Args&&... args)
    {
        _container.emplace_front(std::forward<Args>(args)...);
        emit_after_insert(_container.begin(), std::next(_container.begin()));
    }
    
    void pop_front()
    {
        emit_before_erase(_container.begin(), std::next(_container.begin()));
        _container.pop_front();
    }
    
    void push_front(const T& value)
    {
        _container.push_front(value);
        emit_after_insert(_container.begin(), std::next(_container.begin()));
    }
    
    void push_front(T&& value)
    {
        _container.push_front(std::move(value));
        emit_after_insert(_container.begin(), std::next(_container.begin()));
    }
        
    void remove(const T& value)
    { remove_if([&](const value_type& e){ return e == value; }); }
    
    template<typename Predicate>
    void remove_if(Predicate predicate)
    {
        auto it = _container.begin();
        while(it != _container.end())
        {
            if(predicate(*it))
                it = erase(it);
            else
                ++it;
        }
    }        

    template<typename Observed>
    void splice(const_iterator pos, Observed&& other, iterator first, iterator last)
    {
        if(first == last)
            return;

        auto has_n = std::distance(_container.cbegin(), pos);
        auto insert_n = std::distance(first, last);
        _container.splice(const_it_cast(_container, pos), std::forward<Observed>(other), first, last);
        auto begin = std::next(_container.begin(), has_n);
        emit_after_insert(begin, std::next(begin, insert_n));
    }

    template<typename Observed>
    void splice(const_iterator pos, Observed&& other)
    {
        splice(pos, std::forward<Observed>(other), other.begin(), other.end());
    }

    void splice(const_iterator pos, list& other, iterator first, iterator last)
    {
        splice(pos, std::move(other), first, last);
    }

    void splice(const_iterator pos, list&& other, iterator first, iterator last)
    {
        other.emit_before_erase(first, last);
        splice(pos, other._container, first, last);
    }

    void splice(const_iterator pos, list& other)
    {
        splice(pos, std::move(other));
    }

    void splice(const_iterator pos, list&& other)
    {
        splice(pos, other, other.begin(), other.end());
    }

    void splice(const_iterator pos, list& other, iterator it)
    {
        splice(pos, std::move(other), it);
    }

    void splice(const_iterator pos, list&& other, iterator it)
    {
        splice(pos, other, it, std::next(it));
    }
};
    
template<typename T,
         typename Allocator = std::allocator<T>,
         template <typename, typename> class Observed_ = std::list,
         typename Derived_ = void,
         template <typename> class Signal = signal>
inline bool operator==(const list<T, Allocator, Observed_, Derived_, Signal>& lhs,
                       const list<T, Allocator, Observed_, Derived_, Signal>& rhs)
{ return lhs.observed() == rhs.observed(); }
    
template<typename T,
         typename Allocator = std::allocator<T>,
         template <typename, typename> class Observed_ = std::list,
         typename Derived_ = void,
         template <typename> class Signal = signal>
inline bool operator!=(const list<T, Allocator, Observed_, Derived_, Signal>& lhs,
                       const list<T, Allocator, Observed_, Derived_, Signal>& rhs)
{ return lhs.observed() != rhs.observed(); }

template<typename T,
         typename Allocator = std::allocator<T>,
         template <typename, typename> class Observed_ = std::list,
         typename Derived_ = void,
         template <typename> class Signal = signal>
inline bool operator<(const list<T, Allocator, Observed_, Derived_, Signal>& lhs,
                       const list<T, Allocator, Observed_, Derived_, Signal>& rhs)
{ return lhs.observed() < rhs.observed(); }

template<typename T,
         typename Allocator = std::allocator<T>,
         template <typename, typename> class Observed_ = std::list,
         typename Derived_ = void,
         template <typename> class Signal = signal>
inline bool operator<=(const list<T, Allocator, Observed_, Derived_, Signal>& lhs,
                       const list<T, Allocator, Observed_, Derived_, Signal>& rhs)
{ return lhs.observed() <= rhs.observed(); }

template<typename T,
         typename Allocator = std::allocator<T>,
         template <typename, typename> class Observed_ = std::list,
         typename Derived_ = void,
         template <typename> class Signal = signal>
inline bool operator>(const list<T, Allocator, Observed_, Derived_, Signal>& lhs,
                       const list<T, Allocator, Observed_, Derived_, Signal>& rhs)
{ return lhs.observed() > rhs.observed(); }
    
template<typename T,
         typename Allocator = std::allocator<T>,
         template <typename, typename> class Observed_ = std::list,
         typename Derived_ = void,
         template <typename> class Signal = signal>
inline bool operator>=(const list<T, Allocator, Observed_, Derived_, Signal>& lhs,
                       const list<T, Allocator, Observed_, Derived_, Signal>& rhs)
{ return lhs.observed() >= rhs.observed(); }

template<typename T,
         typename Allocator,
         template <typename, typename> class Observed,
         typename Derived,
         template <typename> class Signal,
         typename Predicate>
inline void erase_if(list<T, Allocator, Observed, Derived, Signal>& cont,
                     Predicate&& pred)
{ cont.remove_if(std::forward<Predicate>(pred)); }

template<typename T,
         typename Allocator,
         template <typename, typename> class Observed,
         typename Derived,
         template <typename> class Signal,
         typename U>
inline void erase(list<T, Allocator, Observed, Derived, Signal>& cont,
                  const U& value)
{ erase_if(cont, [&](const T& o){ return o == value; }); }

}
