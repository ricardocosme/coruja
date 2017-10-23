
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <boost/core/lightweight_test.hpp>
#include <boost/serialization/coruja_object.hpp>
#include <boost/serialization/string.hpp>
#include "boost_serialization_tools.hpp"

int main()
{
    using object_t = coruja::object<std::string>;
    
    run([](std::string path)
        {
            object_t obj("something");
            save_file(path, obj);
        },
        [](std::string path)
        {
            object_t obj;
            bool called{false};
            obj.after_change([&called](object_t::value_type s)
                             {
                                 called = true;
                                 BOOST_TEST(s == "something");
                             });
            load_file(path, obj);
            BOOST_TEST(called);
            BOOST_TEST(obj == "something");
        });
}
