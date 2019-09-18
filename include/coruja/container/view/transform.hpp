
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/container/view/container.hpp"
#include "coruja/support/type_traits.hpp"

#include <boost/hof/is_invocable.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/distance.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/view_facade.hpp>

#include <type_traits>

template<typename Rng, typename Transform>
class coruja_transform_view
  : public ranges::view_facade<coruja_transform_view<Rng, Transform>>
{
    template<typename F>
    struct invoke_observer_base : protected Transform
    {
        Transform& as_transform() noexcept
        { return static_cast<Transform&>(*this); }
        
        const Transform& as_transform() const noexcept
        { return static_cast<const Transform&>(*this); }
        
        invoke_observer_base(Transform t, F f)
            : Transform(std::move(t))
            , _f(std::move(f))
        {}
        
        F _f;
    };
    
    template<typename F>
    struct invoke_observer_impl : private invoke_observer_base<F>
    {
        using base = invoke_observer_base<F>;
        using base::base;
        
        template<typename From, typename It>
        void operator()(From& from, It it) 
        {
            using namespace ranges;
            auto rng = coruja_transform_view{from, base::as_transform()};
            base::_f(rng, next(begin(rng), distance(begin(from), it)));
        }
    };
        
    template<typename F>
    struct invoke_observer_by_ref_impl : private invoke_observer_base<F>
    {
        using base = invoke_observer_base<F>;
        using base::base;
        
        template<typename Ref>
        void operator()(Ref&& ref)
        { base::_f(base::as_transform()(std::forward<Ref>(ref))); }
    };
    
    friend ranges::range_access;
    
    Rng _rng;
    ranges::semiregular_t<Transform> _transform;
    
    class cursor
    {
        ranges::range_iterator_t<Rng> _it;
        mutable ranges::semiregular_t<Transform> _transform;
        
    public:
        
        cursor() = default;
        cursor(ranges::range_iterator_t<Rng> it, Transform transform)
            : _it(it)
            , _transform(std::move(transform))
        {}

        coruja::result_of_t<Transform(ranges::range_reference_t<Rng>)>
        read() const
        { return _transform(*_it); }
        
        bool equal(const cursor &that) const
        { return _it == that._it; }
        
        void next()
        { ++_it; }
        
        void prev()
        { --_it; }
        
        std::ptrdiff_t distance_to(const cursor& that) const
        { return ranges::distance(_it, that._it); }
        
        void advance(std::ptrdiff_t n)
        { ranges::advance(_it, n); }
    };
    
    cursor begin_cursor() const { return {_rng.begin(), _transform}; }
    cursor end_cursor() const { return {_rng.end(), _transform}; }

    template<typename F>
    invoke_observer_impl<typename std::decay<F>::type> invoke_observer(F&& f)
    { return {_transform, std::forward<F>(f)}; }
    
    template<typename F>
    invoke_observer_by_ref_impl<typename std::decay<F>::type> invoke_observer_by_ref(F&& f)
    { return {_transform, std::forward<F>(f)}; }
    
public:
    using observed_t = typename Rng::observed_t;
    using for_each_connection_t = typename Rng::for_each_connection_t;
    using before_erase_connection_t = typename Rng::before_erase_connection_t;

    coruja_transform_view() = default;
    
    coruja_transform_view(Rng rng, Transform transform)
        : _rng(std::move(rng))
        , _transform(std::move(transform))
    {}

    const observed_t& observed() const noexcept
    { return _rng.observed(); }
        
    template<typename F>
    coruja::enable_if_is_not_invocable_t<
        for_each_connection_t, F,
        coruja::result_of_t<Transform(ranges::range_reference_t<Rng>)>>
    for_each(F&& f)
    { return _rng.for_each(invoke_observer(std::forward<F>(f))); }
    
    template<typename F>
    coruja::enable_if_is_invocable_t<
        for_each_connection_t, F,
        coruja::result_of_t<Transform(ranges::range_reference_t<Rng>)>>
    for_each(F&& f)
    { return _rng.for_each(invoke_observer_by_ref(std::forward<F>(f))); }
    
    template<typename F>
    coruja::enable_if_is_not_invocable_t<
        before_erase_connection_t, F,
        coruja::result_of_t<Transform(ranges::range_reference_t<Rng>)>>
    before_erase(F&& f)
    { return _rng.before_erase(invoke_observer(std::forward<F>(f))); }
    
    template<typename F>
    coruja::enable_if_is_invocable_t<
        before_erase_connection_t, F,
        coruja::result_of_t<Transform(ranges::range_reference_t<Rng>)>>
    before_erase(F&& f)
    { return _rng.before_erase(invoke_observer_by_ref(std::forward<F>(f))); }
};

namespace coruja { namespace view {

template<typename ObservableErasableRange, typename F>
inline enable_if_t<
    is_observable_erasable_range<
        typename std::decay<ObservableErasableRange>::type>::value,
    coruja_transform_view<decltype(view(std::declval<ObservableErasableRange>())),
                          remove_reference_t<F>>
>
transform(ObservableErasableRange&& rng, F&& f)
{ return {view(rng), std::forward<F>(f)}; }
    
}}
