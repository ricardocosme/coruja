#pragma once

#include "check_equal.hpp"

#include <boost/core/lightweight_test.hpp>
#include <coruja/support/type_traits.hpp>
#include <coruja/support/signal/any_connection.hpp>
#include <range/v3/range_traits.hpp>

#include <initializer_list>
#include <string>
#include <vector>

template<typename C>
struct F_ {
    C operator()(C& v)
    { return std::move(v); }        
};

template<typename T, typename = void>
struct is_sequence_container : std::false_type {};

template<typename T>
struct is_sequence_container<T, coruja::void_t<
decltype(std::declval<T>().emplace_back(typename T::value_type{}))>>
    : std::true_type
{};

template<typename C>
inline
typename std::enable_if<is_sequence_container<C>::value>::type
emplace(C& c, typename C::value_type e)
{ c.emplace_back(std::move(e)); }

template<typename C>
inline
typename std::enable_if<!is_sequence_container<C>::value>::type
emplace(C& c, typename C::value_type e)
{ c.emplace(std::move(e)); }

template<typename V, typename F = F_<V>, typename E = typename V::observed_t>
void run(V pvec, E for_each_expected,
          F&& f = F_<V>{},
          E expected = E{},
          E erased_expected = E{})
{
    if(expected.size() == 0) expected = for_each_expected;
    bool before_erase_called{false};
    bool before_erase_by_ref_called{false};
    {
        auto vec = std::move(pvec);
        bool for_each_called{false};
        bool for_each_by_ref_called{false};
        E ires, rres, ieres, reres;
        vec.for_each
            ([&for_each_called, &ires](V& pvec, typename V::iterator it)
             {
                 emplace(ires, *it);
                 for_each_called = true;
             });
        vec.for_each
            ([&for_each_by_ref_called, &rres](const typename V::value_type& v)
             {
                 emplace(rres, v);
                 for_each_by_ref_called = true;
             });
        vec.before_erase
            ([&before_erase_called, &ieres](V& pvec, typename V::iterator it)
             {
                 emplace(ieres, *it);
                 before_erase_called = true;
             });
        vec.before_erase
            ([&before_erase_by_ref_called, &reres](const typename V::value_type& v)
             {
                 emplace(reres, v);
                 before_erase_by_ref_called = true;
             });
        auto rvec = f(vec);
        BOOST_TEST(for_each_called);
        BOOST_TEST(for_each_by_ref_called);
        BOOST_TEST(ires == for_each_expected);
        BOOST_TEST(rres == for_each_expected);
        if(!erased_expected.empty())
        {
            BOOST_TEST(ieres == erased_expected);
            BOOST_TEST(reres == erased_expected);
        }
        BOOST_TEST(rvec.observed() == expected);
    }
    BOOST_TEST(before_erase_called);
    BOOST_TEST(before_erase_by_ref_called);
}

template<typename E>
struct Fn_ref
{
    template<typename T>
    void operator()(const T& t)
    { emplace(res, t); }

    E& res;
};
template<typename E>
struct Fn_it
{
    template<typename V, typename It>
    void operator()(V&, It it)
    { emplace(res, *it); }

    E& res;
};

template<typename View, typename F, typename E >
void run_view(View pvec, E for_each_expected, F&& f, E erased_expected)
{
    using conn = coruja::scoped_any_connection;
    auto vec = std::move(pvec);
    using iterator = ranges::range_iterator_t<View>;
    E rres, reres;
    E rres_it, reres_it;
    E rres_t, reres_t;
    E rres_it_t, reres_it_t;
    //Lambda
    conn feconn = vec.for_each([&rres](const typename View::value_type& v)
    { emplace(rres, v); });
    conn feconn_it = vec.for_each([&rres_it](View& view, iterator it)
    { emplace(rres_it, *it); });
    conn beccon = vec.before_erase([&reres](const typename View::value_type& v)
    { emplace(reres, v); });
    conn beccon_it = vec.before_erase([&reres_it](View& view, iterator it)
    { emplace(reres_it, *it); });
    //Template
    conn feconn_t = vec.for_each(Fn_ref<E>{rres_t});
    conn feconn_it_t = vec.for_each(Fn_it<E>{rres_it_t});
    conn beccon_t = vec.before_erase(Fn_ref<E>{reres_t});
    conn beccon_it_t = vec.before_erase(Fn_it<E>{reres_it_t});

    auto rvec = f(vec);
    BOOST_TEST(rres == for_each_expected);
    BOOST_TEST(rres_it == for_each_expected);
    BOOST_TEST(rres_t == for_each_expected);
    BOOST_TEST(rres_it_t == for_each_expected);
    if(!erased_expected.empty())
    {
        BOOST_TEST(reres == erased_expected);
        BOOST_TEST(reres_it == erased_expected);
        BOOST_TEST(reres_t == erased_expected);
        BOOST_TEST(reres_it_t == erased_expected);
    }
}

template<typename View, typename F = F_<View>, typename E >
void run_view(View pvec, E for_each_expected, F&& f = F_<View>{})
{ run_view(std::move(pvec), std::move(for_each_expected), std::move(f), E{}, E{}); }
