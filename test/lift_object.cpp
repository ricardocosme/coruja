
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <coruja/object/object.hpp>
#include <coruja/object/object_io.hpp>
#include <coruja/object/transform.hpp>
#include <coruja/object/lift.hpp>

#include <boost/core/lightweight_test.hpp>

#include <sstream>
#include <string>

using namespace coruja;

using object_t = object<bool>;

struct concat {
    std::string operator()(const std::string& s1,
                           const std::string& s2)
    { return s1 + s2; }
};

int main()
{
    static_assert(std::is_same<object_t::observed_t, bool>::value, "");
    static_assert(std::is_same<object_t::value_type, bool>::value, "");
    
    static_assert(is_observable<object_t>::value, "");
    static_assert(is_observable_object<object_t>::value, "");
    
    //default ctor
    {
        object_t();
    }
    
    //lift(rv, lv...) rs is const
    {
        object<std::string> o1{"abc"};
        object<std::string> o2{"def"};
        auto o3 = lift([](const std::string& s1, const std::string& s2)
                       { return s1 + s2; }, o1, o2);
        bool called{false};
        o3.after_change([&called](std::string s)
                        { called = true; });
        o1 = "ABC";
        BOOST_TEST(called);
        BOOST_TEST(o3 == "ABCdef");
        called = false;
        o2 = "DEF";
        BOOST_TEST(called);
        BOOST_TEST(o3 == "ABCDEF");
    }
    
    //lift(rv, lv...) rs is non const
    {
        object<std::string> o1{"abc"};
        object<std::string> o2{"def"};
        auto o3 = lift(concat{}, o1, o2);
        bool called{false};
        o3.after_change([&called](std::string s)
                        { called = true; });
        o1 = "ABC";
        BOOST_TEST(called);
        BOOST_TEST(o3 == "ABCdef");
        called = false;
        o2 = "DEF";
        BOOST_TEST(called);
        BOOST_TEST(o3 == "ABCDEF");
    }
    
    //lift(lv, lv...)
    {
        object<std::string> o1{"abc"};
        object<std::string> o2{"def"};
        concat f{};
        auto o3 = lift(f, o1, o2);
        bool called{false};
        o3.after_change([&called](std::string s)
                        { called = true; });
        o1 = "ABC";
        BOOST_TEST(called);
        BOOST_TEST(o3 == "ABCdef");
        called = false;
        o2 = "DEF";
        BOOST_TEST(called);
        BOOST_TEST(o3 == "ABCDEF");
    }
    
    //lift(lv, lv, rv)
    {
        object<std::string> o1{"abc"};
        object<std::string> o2{"def"};
        concat f{};
        auto o3 = lift(f, o1, transform(o2, [](std::string s){ return s + "!"; }));
        bool called{false};
        o3.after_change([&called](std::string s)
                        { called = true; });
        o1 = "ABC";
        BOOST_TEST(called);
        BOOST_TEST(o3 == "ABCdef!");
        called = false;
        o2 = "DEF";
        BOOST_TEST(called);
        BOOST_TEST(o3 == "ABCDEF!");
    }
    
    //object<bool> && object<bool>
    {
        object_t o1{true};
        object_t o2{false};
        auto o3 = o1 && o2;
        bool called{false};
        o3.after_change([&called](object_t::value_type s)
                        { called = true; });
        o2 = true;
        BOOST_TEST(called);
        BOOST_TEST(o3 == true);
        called = false;
        o1 = false;
        BOOST_TEST(called);
        BOOST_TEST(o3 == false);
    }
 
    //object<bool> && lift_object
    {
        object_t o1{true};
        object_t o2{true};
        object_t o3{false};
        auto o4 = o1 && o2;
        auto o5 = o3 && o4;
        bool called{false};
        o5.after_change([&called](object_t::value_type s)
                        { called = true; });
        o3 = true;
        BOOST_TEST(called);
        BOOST_TEST(o5 == true);
        o1 = false;
        BOOST_TEST(called);
        BOOST_TEST(o4 == false);
        BOOST_TEST(o5 == false);
    }

    //object<string> + object<string>
    {
        object<std::string> o1{"abc"};
        object<std::string> o2{"def"};
        auto o3 = o1 + o2;
        bool called{false};
        o3.after_change([&called](std::string s)
                        { called = true; });
        o1 = "ABC";
        BOOST_TEST(called);
        BOOST_TEST(o3 == "ABCdef");
        called = false;
        o2 = "DEF";
        BOOST_TEST(called);
        BOOST_TEST(o3 == "ABCDEF");
    }
            
    //object<float> - object<float>
    {
        object<int> o1{10};
        object<int> o2{2};
        auto o3 = o1 - o2;
        bool called{false};
        o3.after_change([&called](int s)
                        { called = true; });
        o1 = 20;
        BOOST_TEST(called);
        BOOST_TEST(o3 == 18);
        called = false;
        o2 = 5;
        BOOST_TEST(called);
        BOOST_TEST(o3 == 15);
        
        BOOST_TEST(o3.get() == 15);
        BOOST_TEST(o3.observed() == 15);
    }
    
    //copy ctor
    {
        object<std::string> o1{"abc"};
        object<std::string> o2{"def"};
        auto o3 = o1 + o2;
        bool called{false};
        o3.after_change([&called](std::string s)
                        { called = true; });
        auto o4 = o3;
        o1 = "ABC";
        BOOST_TEST(called);
        BOOST_TEST(o4 == "ABCdef");
        called = false;
        o2 = "DEF";
        BOOST_TEST(called);
        BOOST_TEST(o4 == "ABCDEF");
    }
    
    //copy assignment operator
    {
        object<std::string> o1{"abc"};
        object<std::string> o2{"def"};
        auto o3 = o1 + o2;
        bool called{false};
        o3.after_change([&called](std::string s)
                        { called = true; });
        auto o4 = o2 + o1;
        o4 = o3;
        o1 = "ABC";
        BOOST_TEST(called);
        BOOST_TEST(o4 == "ABCdef");
        called = false;
        o2 = "DEF";
        BOOST_TEST(called);
        BOOST_TEST(o4 == "ABCDEF");
    }
    
    //move ctor
    {
        object<std::string> o1{"abc"};
        object<std::string> o2{"def"};
        auto o3 = o1 + o2;
        bool called{false};
        o3.after_change([&called](std::string s)
                        { called = true; });
        auto o4 = std::move(o3);
        o1 = "ABC";
        BOOST_TEST(called);
        BOOST_TEST(o4 == "ABCdef");
        called = false;
        o2 = "DEF";
        BOOST_TEST(called);
        BOOST_TEST(o4 == "ABCDEF");
    }
    
    //move assignment operator
    {
        object<std::string> o1{"abc"};
        object<std::string> o2{"def"};
        auto o3 = o1 + o2;
        bool called{false};
        o3.after_change([&called](std::string s)
                        { called = true; });
        auto o4 = o2 + o1;
        o4 = std::move(o3);
        o1 = "ABC";
        BOOST_TEST(called);
        BOOST_TEST(o4 == "ABCdef");
        called = false;
        o2 = "DEF";
        BOOST_TEST(called);
        BOOST_TEST(o4 == "ABCDEF");
    }

    //for_each
    {
        object<bool> o1{true};
        object<bool> o2{true};
        auto o1ANDo2 = o1 && o2;
        int n{0};
        o1ANDo2.for_each([&n](bool v){ ++n; });
        BOOST_TEST(n == 1);
    }
}
