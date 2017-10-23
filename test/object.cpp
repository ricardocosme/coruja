
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <coruja/object/object.hpp>
#include <coruja/object/object_io.hpp>

#include <boost/core/lightweight_test.hpp>

#include <sstream>
#include <string>

using namespace coruja;

using object_t = object<std::string>;

struct custom_string : coruja::object<std::string, custom_string>
{
    using base = coruja::object<std::string, custom_string>;
    using base::base;
    using base::operator=;
};

int main()
{
    static_assert(std::is_same<object_t::observed_t, std::string>::value, "");
    static_assert(std::is_same<object_t::value_type, std::string>::value, "");
    
    static_assert(is_observable<object_t>::value, "");
    static_assert(is_observable_object<object_t>::value, "");
    
    //default ctor
    {
        object_t();
    }

    //object(lv)
    {
        std::string s{"abc"};
        object_t o(s);
        BOOST_TEST(o == "abc");
    }
    
    //object(rv)
    {
        object_t o("abc");
        BOOST_TEST(o == "abc");
    }
    
    //get()/observed()
    {
        std::string s{"abc"};
        object_t o(s);
        BOOST_TEST(o.get() == "abc");
        BOOST_TEST(o.observed() == "abc");
    }
    
    //operator=(T&&)
    {
        object_t ov;
        bool called{false};
        ov.after_change([&called](const object_t::value_type& v)
                        {
                            called = true;
                            BOOST_TEST(v == "change");
                        });
        ov = "change";
        BOOST_TEST(called);
        BOOST_TEST(ov == "change");
    }
    
    //operator=(T&&) - cbk(object&)
    {
        object_t ov;
        bool called{false};
        ov.after_change([&called, &ov](object_t& v)
                        {
                            called = true;
                            BOOST_TEST(v == "change");
                            BOOST_TEST(&ov == &v);
                        });
        ov = "change";
        BOOST_TEST(called);
        BOOST_TEST(ov == "change");
    }
    
    //operator=(T&&) - cbk(Derived&)
    {
        custom_string ov;
        bool called{false};
        ov.after_change([&called, &ov](custom_string& v)
                        {
                            called = true;
                            BOOST_TEST(v == "change");
                            BOOST_TEST(&ov == &v);
                        });
        ov = "change";
        BOOST_TEST(called);
        BOOST_TEST(ov == "change");
    }
    
    //object(object&&)
    {
        object_t ov;
        bool called{false};
        ov.after_change([&called](const object_t::value_type& v)
                        {
                            called = true;
                            BOOST_TEST(v == "change");
                        });
        ov = "change";
        BOOST_TEST(called);
        called = false;
        auto ov2 = std::move(ov);
        BOOST_TEST(!called);
        ov2 = "change";
        BOOST_TEST(called);
    }
    
    //operator=(object&&)
    {
        object_t ov;
        bool called{false};
        ov.after_change([&called](const object_t::value_type& v)
                        {
                            called = true;
                            BOOST_TEST(v == "change");
                        });
        ov = object_t{"change"};
        BOOST_TEST(!called);
        ov = "another change";
        BOOST_TEST(!called);
    }
    
    //equality and inequality
    {
        object_t ov{"ola"};
        object_t ov2{"ola"};
        BOOST_TEST(ov == ov2);
        ov2 = "change";
        BOOST_TEST(ov != ov2);
        BOOST_TEST(ov2 != "ola");
        BOOST_TEST("ola" != ov2);
        BOOST_TEST(ov == "ola");
        BOOST_TEST("ola" == ov);

        object<char> o1{5};
        object<int> o2{5};
        //Object<T1> == Object<T2>
        BOOST_TEST(o2 == o1);
        BOOST_TEST(o1 == o2);
        o2 = 0;
        //Object<T1> != Object<T2>
        BOOST_TEST(o2 != o1);
        BOOST_TEST(o1 != o2);
    }

    //order
    {
        object<char> o1{0};
        object<int> o2{1};
        //Object<T1> vs ObJect<T2>
        BOOST_TEST(o1 < o2);
        BOOST_TEST(o2 > o1);
        BOOST_TEST(o1 <= o2);
        BOOST_TEST(o2 >= o1);
        //T1 vs ObJect<T2>
        BOOST_TEST(0 < o2);
        BOOST_TEST(1 > o1);
        BOOST_TEST(0 <= o2);
        BOOST_TEST(1 >= o1);
        //Object<T1> vs T
        BOOST_TEST(o1 < 1);
        BOOST_TEST(o2 > 0);
        BOOST_TEST(o1 <= 1);
        BOOST_TEST(o2 >= 0);
    }

    //operator<<
    {
        object_t o("abc");
        std::ostringstream os;
        os << o;
        BOOST_TEST(os.str() == o);
    }

    //operator>>
    {
        object_t o;
        bool called{false};
        o.after_change
            ([&called](std::string s)
             {
                 called = true;
                 BOOST_TEST(s == "abc");
             });
        std::istringstream is("abc");
        is >> o;
        BOOST_TEST(called);
        BOOST_TEST(is.str() == o);
    }
}
