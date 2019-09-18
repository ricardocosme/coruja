
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <coruja/object/object.hpp>
#include <coruja/object/object_io.hpp>
#include <coruja/object/view/transform.hpp>

#include <boost/core/lightweight_test.hpp>

#include <sstream>
#include <string>

using namespace coruja;
using namespace coruja::view;

using object_t = coruja::object<std::string>;

struct append_exclamation {
    std::string operator()(const std::string& s)
    { return s + "!"; }
};

struct append_space {
    std::string operator()(const std::string& s)
    { return s + " "; }
};

struct check_str {
    template<typename T>
    void operator()(T&& s) const
    {
        b = true;
        BOOST_TEST(s == expected);
    }
    bool& b;
    std::string expected;
};

int main()
{
    //default ctor
    {
        object_t();
    }
    
    //lv | rv (rv is const)
    {
        object_t o1;
        auto o2 = o1 | [](const object_t::value_type& s)
                       { return s + "!"; };
        bool called{false};
        o2.after_change(check_str{called, "hello!"});
        o1 = "hello";
        BOOST_TEST(called);
        BOOST_TEST(o2 == "hello!");
    }
    
    //transform(lv, rv) rv is const
    {
        object_t o1;
        auto o2 = transform(o1, [](const object_t::value_type& s)
                                { return s + "!"; });
        bool called{false};
        o2.after_change(check_str{called, "hello!"});
        o1 = "hello";
        BOOST_TEST(called);
        BOOST_TEST(o2 == "hello!");
    }
    
    //transform(lv, rv) rv is non const
    {
        object_t o1;
        auto o2 = transform(o1, append_exclamation{});
        bool called{false};
        o2.after_change(check_str{called, "hello!"});
        o1 = "hello";
        BOOST_TEST(called);
        BOOST_TEST(o2 == "hello!");
    }

    //transform(lv, lv)
    {
        object_t o1;
        append_exclamation f{};
        auto o2 = transform(o1, f);
        bool called{false};
        o2.after_change(check_str{called, "hello!"});
        o1 = "hello";
        BOOST_TEST(called);
        BOOST_TEST(o2 == "hello!");
    }
    
    //transform(transform(object)) (using lv)
    {
        object_t o1;
        auto o2 = transform(o1, append_space{});
        auto o3 = transform(o2, append_exclamation{});
        bool called{false};
        o3.after_change(check_str{called, "hello !"});
        o1 = "hello";
        BOOST_TEST(called);
        BOOST_TEST(o2 == "hello ");
        BOOST_TEST(o3 == "hello !");
    }
    
    //object | transform_object | transform_object (using lv)
    {
        object_t o1;
        auto o2 = o1 | append_space{};
        auto o3 = o2 | append_exclamation{};
        bool called{false};
        o3.after_change(check_str{called, "hello !"});
        o1 = "hello";
        BOOST_TEST(called);
        BOOST_TEST(o2 == "hello ");
        BOOST_TEST(o3 == "hello !");
    }
    
    //transform(transform(object)) (using rv)
    {
        object_t o1;
        
        auto o3 = transform(transform(o1, append_space{}),
                            append_exclamation{});
        bool called{false};
        o3.after_change(check_str{called, "hello !"});
        o1 = "hello";
        BOOST_TEST(called);
        BOOST_TEST(o3 == "hello !");
    }
    
    //object | transform_object | transform_object (using rv)
    {
        object_t o1;
        
        auto o3 = o1 | append_space{} | append_exclamation{};
        bool called{false};
        o3.after_change(check_str{called, "hello !"});
        o1 = "hello";
        BOOST_TEST(called);
        BOOST_TEST(o3 == "hello !");
    }
    
    //get()/observed()
    {
        object_t o1;
        auto o2 = transform(o1, append_exclamation{});
        o1 = "hello";
        BOOST_TEST(o2.get() == "hello!");
        BOOST_TEST(o2.observed() == o2.get());
    }
    
    //copy ctor
    {
        object_t o1;
        auto o2 = transform(o1, append_exclamation{});
        bool called{false};
        o2.after_change(check_str{called, "hello!"});
        auto o2copy = o2;
        o1 = "hello";
        BOOST_TEST(called);
        BOOST_TEST(o2 == "hello!");
        BOOST_TEST(o2copy == "hello!");
    }
    
    //copy assignment operator
    {
        object_t o1;
        auto o2 = transform(o1, append_exclamation{});
        bool called{false};
        o2.after_change(check_str{called, "hello!"});
        auto o2copy = transform(o1, append_exclamation{});
        o2copy = o2;
        o1 = "hello";
        BOOST_TEST(called);
        BOOST_TEST(o2 == "hello!");
        BOOST_TEST(o2copy == "hello!");
    }
    
    //move ctor
    {
        object_t o1;
        auto o2 = transform(o1, append_exclamation{});
        bool called{false};
        o2.after_change(check_str{called, "hello!"});
        auto o2copy = std::move(o2);
        o1 = "hello";
        BOOST_TEST(called);
        BOOST_TEST(o2copy == "hello!");
    }
    
    //move assignment operator
    {
        object_t o1;
        auto o2 = transform(o1, append_exclamation{});
        bool called{false};
        o2.after_change(check_str{called, "hello!"});
        auto o2copy = transform(o1, append_exclamation{});
        o2copy = std::move(o2);
        o1 = "hello";
        BOOST_TEST(called);
        BOOST_TEST(o2copy == "hello!");
    }
    
    //equality and inequality
    {
        object_t o1{"abc"};
        auto o2 = transform(o1, [](object_t::value_type s){ return s; });
        //object<T> == transform_object
        BOOST_TEST(o1 == o2);
        //transform_object<object<T>, T, Transform> == object<T>
        BOOST_TEST(o2 == o1);
        auto o3 = transform(o2, [](object_t::value_type s){ return s + s; });
        //object<T> != transform_object
        BOOST_TEST(o1 != o3);
        //transform_object != object<T> 
        BOOST_TEST(o3 != o1);
        //transform_object == T
        BOOST_TEST(o2 == "abc");
        //T == transform_object
        BOOST_TEST("abc" == o2);
        //transform_object != T
        BOOST_TEST(o2 != "def");
        //T != transform_object
        BOOST_TEST("def" != o2);
    }

    //operator<<
    {
        object_t o("hello");
        auto o2 = transform(o, append_exclamation{});
        std::ostringstream os;
        os << o2;
        BOOST_TEST(os.str() == o2);
    }
}
