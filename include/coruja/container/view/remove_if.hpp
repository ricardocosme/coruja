
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <type_traits>

#include <boost/hof/is_invocable.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_fwd.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/view_facade.hpp>

#include "coruja/container/detail/invoke.hpp"
#include "coruja/container/view/container.hpp"
#include "coruja/support/type_traits.hpp"
#include "coruja/support/signal/signal_id_t.hpp"

template<typename ObservableErasableRange, typename Pred>
struct coruja_remove_if_view : ranges::view_facade<
        coruja_remove_if_view<ObservableErasableRange, Pred>>
{
private:
    // <ObservableErasableRangeView>
    template<typename F>
    struct invoke_observer_by_ref_impl : private coruja::detail::invoke_observer_base<F,Pred>
    {
        using base = coruja::detail::invoke_observer_base<F,Pred>;
        using base::base;

        template<typename Ref>
        void operator()(Ref&& ref)
        {;
            if(!base::as_invoker()(ref))
                base::_f(std::forward<Ref>(ref));
        }
    };

    template<typename F>
    invoke_observer_by_ref_impl<typename std::decay<F>::type> invoke_observer_by_ref(F&& f)
    { return {_pred, std::forward<F>(f)}; }

public:

    using Rng = ObservableErasableRange;

    template<typename F, typename Ret>
    using by_ref_t = typename std::enable_if<
                        boost::hof::is_invocable<
                            F, ranges::range_reference_t<Rng>
                        >::value,Ret>::type;

    using value_type = typename Rng::value_type;
    using observed_t = typename Rng::observed_t;
    using for_each_connection_t = typename Rng::for_each_connection_t;
    using before_erase_connection_t = typename Rng::before_erase_connection_t;
    using signal_id_t = coruja::signal_id_t;

    const observed_t& observed() const noexcept
    { return _rng.observed(); }

    template<typename F>
    by_ref_t<F,for_each_connection_t> for_each(F&& f)
    { return _rng.for_each(invoke_observer_by_ref(std::forward<F>(f))); }

    template<typename F>
    by_ref_t<F,before_erase_connection_t> before_erase(F&& f)
    { return _rng.before_erase(invoke_observer_by_ref(std::forward<F>(f))); }

    //Experimental
    signal_id_t for_each_id() const noexcept
    { return _rng.for_each_id(); }

    //Experimental
    signal_id_t before_erase_id() const noexcept
    { return _rng.before_erase_id(); }
    // </ObservableErasableRangeView>

    // <range>
    friend ranges::range_access;

    class cursor
    {
        using iterator = ranges::range_iterator_t<Rng>;
        iterator _it, _begin, _end;
        mutable ranges::semiregular_t<Pred> _pred;
    public:

        void satisfyForward(iterator& it)  { while( it != _end && _pred(*it) ) ++it; }
        void satisfyBackward(iterator& it)  { while( it != _begin && _pred(*it) ) --it; }

        cursor() = default;
        cursor(iterator it, iterator begin, iterator end, ranges::semiregular_t<Pred> pred)
            : _it(it), _begin(begin), _end(end)
            , _pred(std::move(pred))
        { if(_it == _begin) satisfyForward(_it); }

        ranges::range_reference_t<Rng> read() const
        { return *_it; }

        bool equal(const cursor &that) const
        { return _it == that._it; }

        void next()
        { satisfyForward(++_it); }

        CONCEPT_REQUIRES(ranges::BidirectionalRange<Rng>())
        void prev()
        { satisfyBackward(--_it); }

    };

    cursor begin_cursor() const { return {_rng.begin(), _rng.begin(), _rng.end(), _pred}; }
    cursor end_cursor() const { return {_rng.end(), _rng.begin(), _rng.end(), _pred}; }
    // </range>

    coruja_remove_if_view() = default;

    coruja_remove_if_view(Rng&& rng, Pred pred)
        : _rng(std::forward<Rng>(rng))
        , _pred(std::move(pred))
    {}

private:
    Rng _rng;
    ranges::semiregular_t<Pred> _pred;
};

namespace coruja { namespace view {

struct remove_if_fn
{
    template<typename ObservableErasableRange, typename F>
    inline enable_if_t<
        is_observable_erasable_range<
            typename std::decay<ObservableErasableRange>::type>::value,
        coruja_remove_if_view<view_t<ObservableErasableRange>,
                              remove_reference_t<F>>
    >
    operator()(ObservableErasableRange&& rng, F&& f) const
    { return {view(std::forward<ObservableErasableRange>(rng)), std::forward<F>(f)}; }

    template<typename Pred>
    auto operator()(Pred pred) const
    RANGES_DECLTYPE_AUTO_RETURN
    (
        ranges::make_pipeable(std::bind(*this, std::placeholders::_1,
            ranges::protect(std::move(pred))))
    )
};

RANGES_INLINE_VARIABLE(remove_if_fn, remove_if)

}}
