
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <coruja/object/object.hpp>
#include <coruja/object/for_each.hpp>
#include <coruja/object/transform.hpp>

#include <boost/core/lightweight_test.hpp>
#include <string>

using namespace coruja;

struct F {
    void operator()(bool o) const
    { b1 = true; }
    void operator()(std::string o) const
    { b2 = true; }
    bool &b1, &b2;
};

int main() {
    {
        object<bool> o1{false}, o2{true};
        object<std::string> o3{"abc"};
        bool b1{false}, b2{false};
        auto c = for_each(F{b1, b2}, o1, o2, o3);
        BOOST_TEST(b1);
        BOOST_TEST(b2);
        b1 = b2 = false;
        o1 = true;
        BOOST_TEST(b1);
        b1 = false;
        o2 = true;
        BOOST_TEST(b1);
        b1 = false;
        o3 = "def";
        BOOST_TEST(b2);
        b2 = false;
    
        c.disconnect();
    
        o1 = true;
        BOOST_TEST(!b1);
        o2 = true;
        BOOST_TEST(!b1);
        o3 = "ghi";
        BOOST_TEST(!b2);
    }

    //ObservableObject as an rvalue
    {
        object<bool> o1{false};
        bool b1{false}, b2{false};
        for_each(F{b1, b2},
                 o1,
                 transform(o1, [](bool v) -> std::string
                          {return v ? "true" : "false";}));
        BOOST_TEST(b1);
        BOOST_TEST(b2);
        b1 = b2 = false;
        o1 = true;
        BOOST_TEST(b2);
        b2 = false;
    }
}
