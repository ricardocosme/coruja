#include <coruja/boost_optional.hpp>
#include <coruja/support/type_traits.hpp>

#include <boost/core/lightweight_test.hpp>

#include <string>

using namespace coruja;

using optional_t = optional<std::string>;

int main()
{
    static_assert(std::is_same<optional_t::observed_t,
                  boost::optional<std::string>>::value, "");
    
    static_assert(is_observable<optional_t>::value, "");
    static_assert(is_observable_object<optional_t>::value, "");
    
    //default ctor
    {
        optional_t v;
        static_assert(std::is_nothrow_default_constructible<optional_t>::value, "");
    }

    //optiona(observed_t)
    {
        optional_t v(boost::optional<std::string>("abc"));
    }
    
    //optional(none)
    {
        optional_t v(boost::none);
        BOOST_TEST(!v);
    }
    
    //optional(T)
    {
        optional_t v("abc");
        BOOST_TEST(v);
        BOOST_TEST(v.get() == "abc");
    }
    
    //optional(true, T)
    {
        optional_t v(true, "abc");
        BOOST_TEST(v);
        BOOST_TEST(v.get() == "abc");
    }
    
    //optional(false, T)
    {
        optional_t v(false, "abc");
        BOOST_TEST(!v);
    }
    
    //optional(optional<U>&&)
    {
        coruja::optional<std::string> v(coruja::optional<const char*>("abc"));
        BOOST_TEST(v);
        BOOST_TEST(v.get() == "abc");
    }
    
    //optional=(optional<U>&&)
    {
        coruja::optional<std::string> v;
        bool called{true};
        v.after_change
            ([&called](optional_t::observed_t)
             { called = true; });
        v = coruja::optional<const char*>("abc");
        BOOST_TEST(v);
        BOOST_TEST(called);
        BOOST_TEST(v.get() == "abc");
    }

    //optional=(T)
    {
        coruja::optional<std::string> v;
        bool called{true};
        v.after_change
            ([&called](optional_t::observed_t)
             { called = true; });
        v = std::string{"abc"};
        BOOST_TEST(v);
        BOOST_TEST(called);
        BOOST_TEST(v.get() == "abc");
    }

    //optional=(boost::none_t)
    {
        coruja::optional<std::string> v;
        bool called{true};
        v.after_change
            ([&called](optional_t::observed_t)
             { called = true; });
        v = boost::none;
        BOOST_TEST(called);
        BOOST_TEST(!v);
    }
    
    //optional(optional&&)
    {
        coruja::optional<std::string> v;
        bool called{true};
        v.after_change
            ([&called](optional_t::observed_t)
             { called = true; });
        auto v2 = std::move(v);
        v2 = "abc";
        BOOST_TEST(called);
        BOOST_TEST(v2);
        BOOST_TEST(v2.get() == "abc");
    }

    //optional=(optional&&)
    {
        coruja::optional<std::string> v;
        bool called{true};
        v.after_change
            ([&called](optional_t::observed_t)
             { called = true; });
        coruja::optional<std::string> v2;
        v2 = std::move(v);
        v2 = "abc";
        BOOST_TEST(called);
        BOOST_TEST(v2);
        BOOST_TEST(v2.get() == "abc");
    }

    //get() const
    {
        const optional_t o("abc");        
        BOOST_TEST(o);
        BOOST_TEST(o.get() == "abc");
    }
}
