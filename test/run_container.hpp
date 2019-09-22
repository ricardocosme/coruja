#pragma once

#include "check_equal.hpp"

#include <boost/core/lightweight_test.hpp>
#include <coruja/support/type_traits.hpp>
#include <coruja/support/signal/any_connection.hpp>

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

template<typename View, typename F, typename E >
void run_view(View pvec, E for_each_expected, F&& f, E erased_expected)
{
    using conn = coruja::scoped_any_connection;
    auto vec = std::move(pvec);
    E rres, reres;
    E rres_it, reres_it;
    conn feconn = vec.for_each([&rres](const typename View::value_type& v)
    { emplace(rres, v); });
    conn feconn_it = vec.for_each([&rres_it](View& view, typename View::iterator it)
    { emplace(rres_it, *it); });
    conn beccon = vec.before_erase([&reres](const typename View::value_type& v)
    { emplace(reres, v); });
    conn beccon_it = vec.before_erase([&reres_it](View& view, typename View::iterator it)
    { emplace(reres_it, *it); });
    auto rvec = f(vec);
    BOOST_TEST(rres == for_each_expected);
    BOOST_TEST(rres_it == for_each_expected);
    if(!erased_expected.empty())
    {
        BOOST_TEST(reres == erased_expected);
        BOOST_TEST(reres_it == erased_expected);
    }
}

template<typename View, typename F = F_<View>, typename E >
void run_view(View pvec, E for_each_expected, F&& f = F_<View>{})
{ run_view(std::move(pvec), std::move(for_each_expected), std::move(f), E{}, E{}); }
