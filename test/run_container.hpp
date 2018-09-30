#pragma once

#include "check_equal.hpp"

#include <boost/core/lightweight_test.hpp>
#include <coruja/support/type_traits.hpp>

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
