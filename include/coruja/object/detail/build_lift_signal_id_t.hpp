
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "coruja/support/signal/any_signal_id_t.hpp"

#include <boost/variant/apply_visitor.hpp>

#include <algorithm>

namespace coruja { namespace detail {

struct push_back
{
    using result_type = void;
    result_type operator()(signal_id_t o) const
    { ids.emplace_back(o.id()); }
    result_type operator()(lift_signal_id_t o) const
    { std::copy(o.id().begin(), o.id().end(), std::back_inserter(ids)); }
    std::vector<void*>& ids;
};

struct build_lift_signal_id_t
{
    void resolve_signal_t(any_signal_id_t s) const
    { boost::apply_visitor(push_back{ids}, std::move(s)); }

    template<typename SignalId>
    void resolve_signal_t(SignalId&& s) const
    {  push_back{ids}(std::forward<SignalId>(s)); }

    template<typename Observable>
    void operator()(Observable& o) const
    {  resolve_signal_t(o.after_change_id()); }
    std::vector<void*>& ids;
};

}}

