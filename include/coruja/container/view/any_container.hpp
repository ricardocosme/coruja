
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <iterator>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

#include "coruja/container/view/container.hpp"
#include "coruja/support/signal/any_connection.hpp"
#include "coruja/support/signal/any_signal_id_t.hpp"
#include "coruja/support/type_traits.hpp"
#include "range/v3/view/any_view.hpp"
#include <range/v3/range_traits.hpp>


namespace coruja { namespace view {

template<typename T, ranges::category Cat = ranges::category::input>
class any_container : public ranges::any_view<T, Cat>
{
    using base_view = ranges::any_view<T, Cat>;
public:
    using iterator = ranges::range_iterator_t<base_view>;
    using value_type = typename std::decay<T>::type;
    using for_each_connection_t = any_connection;
    using before_erase_connection_t = any_connection;
private:

    struct any_container_iface
    {
        virtual ~any_container_iface() = default;
        virtual any_container_iface* clone() const = 0;

        virtual for_each_connection_t for_each(std::function<void(value_type)>) = 0;
        virtual for_each_connection_t for_each(std::function<void(any_container&,iterator)>) = 0;

        virtual before_erase_connection_t before_erase(std::function<void(value_type)>) = 0;
        virtual before_erase_connection_t before_erase(std::function<void(any_container&,iterator)>) = 0;
    };

    struct wrap_invoker
    {
        template<typename From, typename It>
        void operator()(From& from, It it) 
        {
            using namespace ranges;
            auto dist = distance(begin(from), it);
            auto rng = any_container{from};
             _f(rng, next(begin(rng), dist));
        }

        std::function<void(any_container&,iterator)> _f;
    };

    template<typename ObservableEresableRange>
    struct model_t : any_container_iface
    {
        model_t() = default;

        template<typename U>
        model_t(U&& u) : _rng(std::forward<U>(u)) {}

        virtual ~model_t() = default;

        any_container_iface* clone() const override
        { return new model_t(_rng); }

        for_each_connection_t for_each(std::function<void(value_type)> f) override
        { return _rng.for_each(std::move(f)); }
        for_each_connection_t for_each(std::function<void(any_container&,iterator)> f) override
        { return _rng.for_each(wrap_invoker{std::move(f)}); }

        before_erase_connection_t before_erase(std::function<void(value_type)> f) override
        { return _rng.before_erase(std::move(f)); }
        before_erase_connection_t before_erase(std::function<void(any_container&,iterator)> f) override
        { return _rng.before_erase(wrap_invoker{std::move(f)}); }

        ObservableEresableRange _rng;
    };

    std::unique_ptr<any_container_iface> _model;

public:
    any_container() = default;

    template<typename ObservableEresableRange,
             typename Enable = enable_if_t<
                 !std::is_same<
                     typename std::decay<ObservableEresableRange>::type,
                     any_container>::value
                 >>
    any_container(ObservableEresableRange&& o)
        : base_view(o)
        , _model(new model_t<view_t<ObservableEresableRange>>(view(o)))
    {}

    any_container(const any_container& rhs)
        : base_view(rhs)
        , _model(rhs._model ? rhs._model->clone() : nullptr)
    {}

    any_container& operator=(const any_container& rhs)
    {
        base_view::operator=(rhs);
        _model.reset(rhs._model ? rhs._model->clone() : nullptr);
        return *this;
    }

    any_container(any_container&& rhs) = default;
    any_container& operator=(any_container&&) = default;

    template<typename F>
    for_each_connection_t for_each(F&& f)
    {  return _model->for_each(std::forward<F>(f)); }

    template<typename F>
    before_erase_connection_t before_erase(F&& f)
    { return _model->before_erase(std::forward<F>(f)); }
};

template<typename T>
using any_input_container = any_container<T, ranges::category::input>;

template<typename T>
using any_forward_container = any_container<T, ranges::category::forward>;

template<typename T>
using any_bidirectional_container = any_container<T, ranges::category::bidirectional>;

template<typename T>
using any_random_access_container = any_container<T, ranges::category::random_access>;

}}