
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/core/lightweight_test.hpp>
#include <initializer_list>

template<typename ocontainer, typename value_type>
inline void check_equal(ocontainer& c, std::initializer_list<value_type> il)
{
    BOOST_TEST(c.size() == il.size());
    auto it = il.begin();
    for(auto&& e : c)
        BOOST_TEST(e == *it++);
}

// template<typename ocontainer, typename expected_t>
// void check_equal(const ocontainer& c, const expected_t& expected)
// {
//     BOOST_TEST(c.size() == expected.size());
//     for(auto& e : c) BOOST_TEST(expected.at(e.first) == e.second);
// }
