
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <boost/core/lightweight_test.hpp>
#include <boost/serialization/string.hpp>

#include <boost/serialization/coruja_object.hpp>
#include <boost/serialization/coruja_list.hpp>
#include <coruja/object/object.hpp>
#include "boost_serialization_tools.hpp"
#include "check_equal.hpp"

using namespace coruja;

std::initializer_list<std::string> expected{"abc", "def", "ghi"};

struct my_list : list<std::string,
                      std::allocator<std::string>,
                      std::list,
                      my_list>
{
    using base = list<std::string,
                      std::allocator<std::string>,
                      std::list,
                      my_list>;
    using base::base;

    template<typename Archive>
    void serialize(Archive& ar, unsigned int)
    { ar & static_cast<base&>(*this); }
};

int main()
{
    run([](std::string path)
        {
            list<std::string> obj{expected};
            save_file(path, obj);
        },
        [](std::string path)
        {
            list<std::string> obj;
            bool called{false};
            std::list<std::string> vcbk;
            obj.for_each([&called, &vcbk](std::string s)
                         {
                             called = true;
                             vcbk.push_back(s);
                         });
            load_file(path, obj);
            BOOST_TEST(called);
            check_equal(obj, expected);
            check_equal(vcbk, expected);
        });
            
    run([](std::string path)
        {
            my_list obj{expected};
            save_file(path, obj);
        },
        [](std::string path)
        {
            my_list obj;
            bool called{false};
            std::list<std::string> vcbk;
            obj.for_each([&called, &vcbk](std::string s)
                         {
                             called = true;
                             vcbk.push_back(s);
                         });
            load_file(path, obj);
            BOOST_TEST(called);
            check_equal(obj, expected);
            check_equal(vcbk, expected);
        });
}
