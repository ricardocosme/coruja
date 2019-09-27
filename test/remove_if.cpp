
// Copyright Hugo Machado 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "check_equal.hpp"

#include <coruja/container/algorithm/remove_if.hpp>

#include <vector>

std::initializer_list<int> data{1, 2, 3, 4, 5, 6};

int main() {

    //Empty container
    {
        std::vector<int> numbers{};
        auto iterator = coruja::remove_if(numbers.begin(), numbers.end(),
        [](int number)
        { return number < 4; });

        check_equal(numbers, std::initializer_list<int>{});
        BOOST_TEST(iterator == numbers.begin());
        BOOST_TEST(iterator == numbers.end());
    }

    //No match for predicate
    {
        std::vector<int> numbers{data};
        auto iterator = coruja::remove_if(numbers.begin(), numbers.end(),
        [](int number)
        { return false; });

        check_equal(numbers, data);
        BOOST_TEST(iterator == numbers.end());
    }

    //Predicate matches partially
    {
        std::vector<int> numbers{data};
        auto iterator = coruja::remove_if(numbers.begin(), numbers.end(),
        [](int number)
        { return number < 4; });

        check_equal(numbers, {4, 5, 6, 1, 2, 3});
        BOOST_TEST(iterator == numbers.begin() + 3);
    }

    //Predicate always match
    {
        std::vector<int> numbers{data};
        auto iterator = coruja::remove_if(numbers.begin(), numbers.end(),
        [](int number)
        { return true; });

        check_equal(numbers, data);
        BOOST_TEST(iterator == numbers.begin());
    }
}
