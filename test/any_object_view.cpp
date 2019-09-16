
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <coruja/object/any_object_view.hpp>
#include <coruja/object/lift.hpp>
#include <coruja/object/object.hpp>
#include <coruja/object/object_io.hpp>
#include <coruja/object/transform.hpp>

#include <boost/core/lightweight_test.hpp>
#include <sstream>

using namespace coruja;

using any_object_t = any_object_view<int>;
using object_t = object<int>;

int main()
{
    static_assert(std::is_same<object_t::observed_t, int>::value, "");
    static_assert(std::is_same<object_t::value_type, int>::value, "");
    
    static_assert(is_observable<any_object_t>::value, "");
    static_assert(is_observable_object<any_object_t>::value, "");

    //default ctor
    {
        any_object_t o;
    }

    //any_object_view(lv = object)
    {
        object_t o(1);
        any_object_t any(o);
        BOOST_TEST(any == 1);
        bool called{false};
        any.after_change
            ([&called](int i)
             {
                 called = true;
                 BOOST_TEST(i == 5);
             });
        o = 5;
        BOOST_TEST(called);
        BOOST_TEST(any == 5);
    }

    //any_object_view(lv = transform)
    {
        object_t o(1);
        auto transf = o | [](const object_t::value_type& v){ return v; };
        any_object_t any(transf);
        BOOST_TEST(any == 1);
        bool called{false};
        any.after_change
            ([&called](int i)
             {
                 called = true;
                 BOOST_TEST(i == 5);
             });
        o = 5;
        BOOST_TEST(called);
        BOOST_TEST(any == 5);
    }

    //any_object_view(rv = transform)
    {
        object_t o(1);
        any_object_t any(o | [](const object_t::value_type& v){ return v; });
        BOOST_TEST(any == 1);
        bool called{false};
        any.after_change
            ([&called](int i)
             {
                 called = true;
                 BOOST_TEST(i == 5);
             });
        o = 5;
        BOOST_TEST(called);
        BOOST_TEST(any == 5);
    }
    
    //any_object_view(rv)
    {
        object_t o1, o2;
        o1 = 0;
        o2 = 0;
        any_object_t any;
        bool called{false};
        {
            auto rv = o1 + o2;
            any = std::move(rv);
        }
        any.after_change
            ([&called](int i)
             {
                 called = true;
                 BOOST_TEST(i == 5);
             });
        o1 = 5;
        BOOST_TEST(called);
        BOOST_TEST(any == 5);
    }
    
    //get()/observed()
    {
        object_t o(1);
        any_object_t any(o);
        BOOST_TEST(any.get() == 1);
        BOOST_TEST(any.observed() == any.get());
    }

    //any_object_view(any_object_view&&)
    {
        object_t o;
        any_object_t any(o);
        bool called{false};
        any.after_change([&called](int i)
                         {
                             called = true;
                             BOOST_TEST(i == 1);
                         });
        auto any2 = std::move(any);
        o = 1;
        BOOST_TEST(any2.get() == 1);
        BOOST_TEST(called);
    }
    
    //any_object_view& operator=(any_object_view&&)
    {
        object_t o;
        any_object_t any(o);
        bool called{false};
        any.after_change([&called](int i)
                         {
                             called = true;
                             BOOST_TEST(i == 1);
                         });
        any_object_t any2;
        any2 = std::move(any);
        o = 1;
        BOOST_TEST(any2.get() == 1);
        BOOST_TEST(called);
    }
    
    //any_object_view(const any_object_view&)
    {
        object_t o;
        any_object_t any(o);
        bool called{false};
        any.after_change([&called](int i)
                         {
                             called = true;
                             BOOST_TEST(i == 1);
                         });
        auto any2 = any;
        o = 1;
        BOOST_TEST(any.get() == 1);
        BOOST_TEST(any2.get() == 1);
        BOOST_TEST(any2 == any);
        BOOST_TEST(called);
    }
    
    //any_object_view& operator=(const any_object_view&)
    {
        object_t o;
        any_object_t any(o);
        bool called{false};
        any.after_change([&called](int i)
                         {
                             called = true;
                             BOOST_TEST(i == 1);
                         });
        any_object_t any2;
        any2 = any;
        o = 1;
        BOOST_TEST(any.get() == 1);
        BOOST_TEST(any2.get() == 1);
        BOOST_TEST(any2 == any);
        BOOST_TEST(called);
    }

    //equality and inequality
    {
        object<std::string> _ov{"ola"};
        object<std::string> _ov2{"ola"};
        any_object_view<std::string> ov = _ov;
        any_object_view<std::string> ov2 = _ov2;
        BOOST_TEST(ov == ov2);
        _ov2 = "change";
        BOOST_TEST(ov != ov2);
        BOOST_TEST(ov2 != "ola");
        BOOST_TEST("ola" != ov2);
        BOOST_TEST(ov == "ola");
        BOOST_TEST("ola" == ov);

        object<char> _o1{5};
        object<int> _o2{5};
        any_object_view<char> o1 = _o1;
        any_object_view<int> o2 = _o2;
        //Object<T1> == Object<T2>
        BOOST_TEST(o2 == o1);
        BOOST_TEST(o1 == o2);
        _o2 = 0;
        //Object<T1> != Object<T2>
        BOOST_TEST(o2 != o1);
        BOOST_TEST(o1 != o2);
    }

    //order
    {
        object<char> _o1{0};
        object<int> _o2{1};
        any_object_view<char> o1 = _o1;
        any_object_view<int> o2 = _o2;
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
        object<std::string> o("abc");
        any_object_view<std::string> any(o);
        std::ostringstream os;
        os << any;
        BOOST_TEST(os.str() == any);
    }
}
