
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/container/detail/const_it_cast.hpp"
#include "coruja/container/detail/for_each.hpp"
#include "coruja/support/signal.hpp"

#include <boost/hof/is_invocable.hpp>

namespace coruja {

template<typename Observed,
         typename Derived,
         template <typename> class Signal>    
class container
{
public:
    
    using observed_t = Observed;
    
protected:
        
    Derived& as_derived() noexcept
    { return static_cast<Derived&>(*this); }
    
    const Derived& as_derived() const noexcept
    { return static_cast<const Derived&>(*this); }
    
    using after_insert_rng_t = Signal<void(Derived&,
                                           typename observed_t::iterator,
                                           typename observed_t::iterator)>;
    using before_erase_rng_t = Signal<void(Derived&,
                                           typename observed_t::iterator,
                                           typename observed_t::iterator)>;

    before_erase_rng_t _before_erase;
    after_insert_rng_t _after_insert;
    
    void emit_after_insert(typename observed_t::iterator first,
                           typename observed_t::iterator last)
    { _after_insert(as_derived(), first, last); }
    
    void emit_before_erase(typename observed_t::iterator first,
                           typename observed_t::iterator last)
    { _before_erase(as_derived(), first, last); }
    
    observed_t _container;
    
public:
    
    using for_each_connection_t = typename after_insert_rng_t::connection_t;
    using before_erase_connection_t = typename before_erase_rng_t::connection_t;

    using value_type = typename Observed::value_type;
    using reference = typename Observed::reference;
    using const_reference = typename Observed::const_reference;
    using iterator = typename Observed::iterator;
    using const_iterator = typename Observed::const_iterator;
    using difference_type = typename Observed::difference_type;
    using size_type = typename Observed::size_type;

    container() = default;
    
    explicit container(observed_t observed)
        : _container(std::move(observed))
    {}
    
    iterator begin() noexcept
    { return _container.begin(); }
    
    const_iterator begin() const noexcept
    { return _container.begin(); }
    
    const_iterator cbegin() const noexcept
    { return _container.cbegin(); }
    
    iterator end() noexcept
    { return _container.end(); }
    
    const_iterator end() const noexcept
    { return _container.end(); }
    
    const_iterator cend() const noexcept
    { return _container.cend(); }

    size_type size() const noexcept
    { return _container.size(); }
    
    size_type max_size() const noexcept
    { return _container.max_size(); }
    
    bool empty() const noexcept
    { return _container.empty(); }

    void clear() noexcept
    {
        emit_before_erase(_container.begin(), _container.end());
        _container.clear();
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
    
    void swap(Derived& other) noexcept
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

    void swap(observed_t& otherContainer) noexcept
    {
        if(!_container.empty())
            emit_before_erase(begin(), end());
        _container.swap(otherContainer);
        emit_after_insert(begin(), end());
    }
    
    const observed_t& observed() const noexcept
    { return _container; }
    
    template<typename F>
    enable_if_t<
        !boost::hof::is_invocable<
            F, value_type&>::value,
        for_each_connection_t
    >
    for_each(F&& f)
    { return detail::for_each_by<F, detail::fwd_by_it>(as_derived(), std::forward<F>(f)); }
    
    template<typename F>
    enable_if_t<
        boost::hof::is_invocable<
            F, value_type&>::value,
        for_each_connection_t
    >
    for_each(F&& f)
    { return detail::for_each_by<F, detail::fwd_by_ref>(as_derived(), std::forward<F>(f)); }

    template<typename F>
    enable_if_t<
        !boost::hof::is_invocable<
            F, value_type&>::value,
        before_erase_connection_t
    >
    before_erase(F&& f)
    {
        return _before_erase.connect
            (detail::for_each_impl<F, detail::fwd_by_it<F&>>{std::forward<F>(f)});
    }    
    
    template<typename F>
    enable_if_t<
        boost::hof::is_invocable<
            F, value_type&>::value,
        before_erase_connection_t
    >
    before_erase(F&& f)
    {
        return _before_erase.connect
            (detail::for_each_impl<F, detail::fwd_by_ref<F&>>{std::forward<F>(f)});
    }

    friend bool operator==(const Derived& lhs, const Derived& rhs)
    { return lhs.observed() == rhs.observed(); }

    friend bool operator==(const Derived& lhs, const observed_t& rhs)
    { return lhs.observed() == rhs; }
    
    friend bool operator==(const observed_t& lhs, const Derived& rhs)
    { return lhs == rhs.observed(); }
    
    friend bool operator!=(const Derived& lhs, const Derived& rhs)
    { return !(lhs == rhs); }

    friend bool operator!=(const Derived& lhs, const observed_t& rhs)
    { return !(lhs == rhs); }
    
    friend bool operator!=(const observed_t& lhs, const Derived& rhs)
    { return !(lhs == rhs); }

    friend bool operator<(const Derived& lhs, const Derived& rhs)
    { return lhs.observed() < rhs.observed(); }

    friend bool operator<(const Derived& lhs, const observed_t& rhs)
    { return lhs.observed() < rhs; }
    
    friend bool operator<(const observed_t& lhs, const Derived& rhs)
    { return lhs.observed() < rhs; }

    friend bool operator>(const Derived& lhs, const Derived& rhs)
    { return rhs < lhs; }

    friend bool operator>(const Derived& lhs, const observed_t& rhs)
    { return rhs < lhs; }
    
    friend bool operator>(const observed_t& lhs, const Derived& rhs)
    { return rhs < lhs; }
    
    friend bool operator<=(const Derived& lhs, const Derived& rhs)
    { return !(rhs < lhs); }

    friend bool operator<=(const Derived& lhs, const observed_t& rhs)
    { return !(rhs < lhs); }
    
    friend bool operator<=(const observed_t& lhs, const Derived& rhs)
    { return !(rhs < lhs); }
    
    friend bool operator>=(const Derived& lhs, const Derived& rhs)
    { return !(lhs < rhs); }

    friend bool operator>=(const Derived& lhs, const observed_t& rhs)
    { return !(lhs < rhs); }
    
    friend bool operator>=(const observed_t& lhs, const Derived& rhs)
    { return !(lhs < rhs); }
};
    
}
