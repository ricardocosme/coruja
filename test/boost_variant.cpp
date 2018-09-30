
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <coruja/boost_variant.hpp>

#include <boost/core/lightweight_test.hpp>

#include <string>

using namespace coruja;

using variant_t = variant<int, float, std::string>;

template<typename Expected>
struct visitor_t {
    using result_type = void;
    template<typename T>
    result_type operator()(T) const
    { BOOST_TEST(boost::typeindex::type_id<T>()
             == boost::typeindex::type_id<Expected>());
    }
};

struct my_variant : variant<int, float, derived<my_variant>> {
    using base = variant<int, float, derived<my_variant>>;
    using base::base;
};

int main()
{
    static_assert(std::is_same<variant_t::observed_t,
                  boost::variant<int, float, std::string>>::value, "");
    
    static_assert(is_observable<variant_t>::value, "");
    static_assert(is_observable_object<variant_t>::value, "");

    static_assert(std::is_same<my_variant::observed_t,
                  boost::make_variant_over<boost::mpl::vector<int, float>>::type>::value, "");
    
    static_assert(std::is_nothrow_destructible<variant_t>::value,
                  "");
    
    //default ctor
    {
        variant_t v;
        static_assert(std::is_nothrow_default_constructible<variant_t>::value,
                      "");
    }

    //variant(observed_t)
    {
        variant_t v(variant_t::observed_t(5));
    }

    {
        variant_t v;
        v.after_change([](variant_t&){});
    }
    
    //which()
    {
        variant_t v;
        BOOST_TEST(v.which() == 0);
        v = std::string{"something"};
        BOOST_TEST(v.which() == 2);        
    }

    //empty()
    {
        variant_t v;
        BOOST_TEST(!v.empty());
    }

    //variant(U)
    {
        variant_t v(1.0f);
        variant_t v2("abc");
    }

    //visit
    {        
        variant_t v{5.6f};
        v.visit(visitor_t<float>{});
        v.apply_visitor(visitor_t<float>{});
        visitor_t<float> vis;
        v.visit(vis);
        v.apply_visitor(vis);
    }
    
    //const visit
    {        
        const variant_t v{5.6f};
        v.visit(visitor_t<float>{});
        v.apply_visitor(visitor_t<float>{});
    }
}
