#include "check_equal.hpp"
#include "associative_container.hpp"

#include <boost/core/lightweight_test.hpp>
#include <coruja/container/set.hpp>

#include <string>

using namespace coruja;

using cont_t = set<std::string>;

int main()
{
    test_associative_container<cont_t>();
    
    //default ctor
    {
        cont_t cont;
    }
    
    //set(observed_t) rv
    {
        run(cont_t(cont_t::observed_t({"abc", "def"})),
             cont_t::observed_t({"abc", "def"}));
    }
    
    //set(observed_t) lv
    {
        cont_t::observed_t lv({"abc", "def"});
        run(cont_t(lv),
             cont_t::observed_t({"abc", "def"}));
    }
        
    //set(first, last)
    {
        cont_t::observed_t src({"abc", "def"});
        run(cont_t(src.begin(), src.end()),
             cont_t::observed_t({"abc", "def"}));
    }

    //set(il)
    {
        run(cont_t({"abc", "def"}),
             cont_t::observed_t({"abc", "def"}));
    }
    
    //insert(first, last)
    {
        run(cont_t{"abc"},
            {"abc", "def", "ghi"},
            [](cont_t& cont)
            {
                std::initializer_list<std::string> src
                    {"def", "ghi"};
                cont.insert(src.begin(), src.end());
                BOOST_TEST(cont.size() == 3);
                return std::move(cont);
            });
    }
    
    //insert(il)
    {
        run(cont_t{"abc"},
            {"abc", "def", "ghi"},
            [](cont_t& cont)
            {
                cont.insert({"def", "ghi"});
                BOOST_TEST(cont.size() == 3);
                return std::move(cont);
            });
    }
}
