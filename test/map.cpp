#include "check_equal.hpp"
#include "associative_container.hpp"

#include <boost/core/lightweight_test.hpp>
#include <coruja/container/map.hpp>

#include <string>

using namespace coruja;

using cont_t = map<std::string, std::string>;

int main()
{
    test_associative_container<cont_t>();
    
    //default ctor
    {
        cont_t cont;
    }
    
    //map(observed_t) rv
    {
        run(cont_t(cont_t::observed_t({{"a", "abc"}, {"b", "def"}})),
             cont_t::observed_t({{"a", "abc"}, {"b", "def"}}));
    }
    
    //map(observed_t) lv
    {
        cont_t::observed_t lv({{"a", "abc"}, {"b", "def"}});
        run(cont_t(lv),
             cont_t::observed_t({{"a", "abc"}, {"b", "def"}}));
    }
        
    //map(first, last)
    {
        cont_t::observed_t src({{"a", "abc"}, {"b", "def"}});
        run(cont_t(src.begin(), src.end()),
             cont_t::observed_t({{"a", "abc"}, {"b", "def"}}));
    }

    //map(il)
    {
        run(cont_t({{"a", "abc"}, {"b", "def"}}),
             cont_t::observed_t({{"a", "abc"}, {"b", "def"}}));
    }
    
    //insert(first, last)
    {
        run(cont_t{{"k1", "abc"}},
            {{"k1" , "abc"}, {"k2", "def"}, {"k3", "ghi"}},
            [](cont_t& cont)
            {
                std::initializer_list<std::pair<std::string, std::string>> src
                    {{"k2", "def"}, {"k3", "ghi"}};
                cont.insert(src.begin(), src.end());
                BOOST_TEST(cont.size() == 3);
                return std::move(cont);
            });
    }
    
    //insert(il)
    {
        run(cont_t{{"k1", "abc"}},
            {{"k1" , "abc"}, {"k2", "def"}, {"k3", "ghi"}},
            [](cont_t& cont)
            {
                cont.insert({{"k2", "def"}, {"k3", "ghi"}});
                BOOST_TEST(cont.size() == 3);
                return std::move(cont);
            });
    }

    //insert_sorted
    {
        cont_t cont{{"1", "abc"}, {"6", "def"}};
        std::size_t step{0};
        cont.for_each([&](cont_t&, cont_t::iterator fst, cont_t::iterator lst)
        {
            if(step == 0) {
                BOOST_TEST(std::distance(fst, lst) == 2);
                BOOST_TEST(fst->first == "1");
                BOOST_TEST(std::next(fst)->first == "6");
                step = 1;
            } else if(step == 1) {
                BOOST_TEST(std::distance(fst, lst) == 2);
                BOOST_TEST(fst->first == "4");
                BOOST_TEST(std::next(fst)->first == "5");
                step = 2;
            } else if(step == 2) {
                BOOST_TEST(std::distance(fst, lst) == 3);
                BOOST_TEST((fst++)->first == "7");
                BOOST_TEST((fst++)->first == "8");
                BOOST_TEST((fst++)->first == "9");
                step = 3;
            } else abort();
        });
        cont.insert_sorted({{"4", "xxx"}, {"5", "xxx"}});
        std::initializer_list<cont_t::value_type> il{{"7", ""}, {"8", ""}, {"9", ""}};
        cont.insert_sorted(il);
    }
    
    //at fail
    {
        cont_t cont;
        bool ok{false};
        try{ cont.at("k1"); }
        catch(const std::out_of_range&)
        { ok = true; }
        BOOST_TEST(ok);
    }

    //at success
    {
        bool ok{true};
        cont_t cont{{"k1", "abc"}};
        try{ BOOST_TEST(cont.at("k1") == "abc"); }
        catch(const std::out_of_range&)
        { ok = false; }
        BOOST_TEST(ok);
    }

    //at const fail
    {
        cont_t cont;
        bool ok{false};
        try
        {
            const auto& cvalue = cont;
            cvalue.at("k1");
        }
        catch(const std::out_of_range&)
        { ok = true; }
        BOOST_TEST(ok);
    }

    //at const success
    {
        bool ok{true};
        cont_t cont{{"k1", "abc"}};
        try
        {
            const auto& ccont = cont;
            auto cvalue = ccont.at("k1");
            BOOST_TEST(cvalue == "abc");
        }
        catch(const std::out_of_range&)
        { ok = false; }
        BOOST_TEST(ok);            
    }
    
    //operator[lvalue] 
    {
        cont_t cont({{"k1", "abc"}});
        BOOST_TEST(cont["k1"] == "abc");
    }

    //operator[rvalue] 
    {
        cont_t cont({{"k1", "abc"}});
        cont.emplace("k1", "abc");
        BOOST_TEST(cont["k1"] == "abc");
    }
}
