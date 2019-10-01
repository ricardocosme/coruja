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
    
    //insert_sorted
    {
        cont_t cont{"1", "6"};
        std::size_t step{0};
        cont.for_each([&](cont_t&, cont_t::iterator fst, cont_t::iterator lst)
        {
            if(step == 0) {
                BOOST_TEST(std::distance(fst, lst) == 2);
                BOOST_TEST(*fst++ == "1");
                BOOST_TEST(*fst == "6");
                step = 1;
            } else if(step == 1) {
                BOOST_TEST(std::distance(fst, lst) == 2);
                BOOST_TEST(*fst++ == "4");
                BOOST_TEST(*fst == "5");
                step = 2;
            } else if(step == 2) {
                BOOST_TEST(std::distance(fst, lst) == 3);
                BOOST_TEST(*fst++ == "7");
                BOOST_TEST(*fst++ == "8");
                BOOST_TEST(*fst++ == "9");
                step = 3;
            } else abort();
        });
        cont.insert_sorted({"4", "5"});
        std::initializer_list<cont_t::value_type> il{"7", "8", "9"};
        cont.insert_sorted(il);
    }

    //erase_if
    {
        using oset = coruja::set<int>;
        oset s{{1, 2, 3, 4, 5, 6}};
        std::vector<int> removed;
        auto c = s.before_erase([&](int i){ removed.push_back(i); });
        auto is_odd = [](int i){ return i%2; };
        coruja::erase_if(s, is_odd);
        BOOST_TEST((removed == std::vector<int>{1, 3, 5}));
        c.disconnect();
    }

    //erase
    {
        using sset = coruja::set<std::string>;
        sset s({"xxx", "def", "jkl"});
        auto c = s.before_erase([](std::string e)
                                    { BOOST_TEST(e == std::string{"xxx"}); });
        coruja::erase(s, "xxx");
        c.disconnect();
    }
}
