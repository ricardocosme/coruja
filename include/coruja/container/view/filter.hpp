
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <range/v3/utility/functional.hpp>

#include "coruja/container/view/remove_if.hpp"

namespace coruja { namespace view {

struct filter_fn
{
    template<typename ObservableErasableRange, typename Pred>
    inline enable_if_t<
        is_observable_erasable_range<
            typename std::decay<ObservableErasableRange>::type>::value,
        coruja_remove_if_view<view_t<ObservableErasableRange>,
                              ranges::logical_negate<Pred>>
    >
    operator()(ObservableErasableRange&& rng, Pred pred) const
    { return {view(std::forward<ObservableErasableRange>(rng)), ranges::not_fn(std::move(pred))}; }

    template<typename Pred>
    auto operator()(Pred pred) const
    RANGES_DECLTYPE_AUTO_RETURN
    (
        ranges::make_pipeable(std::bind(*this, std::placeholders::_1,
            ranges::protect(std::move(pred))))
    )
};

RANGES_INLINE_VARIABLE(filter_fn, filter)

}}
