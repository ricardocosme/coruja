#include "check_equal.hpp"
#include "associative_container.hpp"

#include <boost/core/lightweight_test.hpp>
#include <coruja/container/unordered_map.hpp>

#include <set>
#include <string>

using namespace coruja;

using cont_t = unordered_map<std::string, std::string>;

int main()
{
    test_associative_container<cont_t>();
    
    //default ctor
    {
        cont_t umap;
    }

    //unordered_map(size_type)
    {
        cont_t cont(2);
        BOOST_TEST(cont.empty());
    }
    
    //unordered_map(observed_t) rv
    {
        run(cont_t(std::unordered_map<std::string, std::string>({{"a", "abc"}, {"b", "def"}})),
             cont_t::observed_t({{"a", "abc"}, {"b", "def"}}));
    }
    
    //unordered_map(observed_t) lv
    {
        std::unordered_map<std::string, std::string> lv({{"a", "abc"}, {"b", "def"}});
        run(cont_t(lv),
             cont_t::observed_t({{"a", "abc"}, {"b", "def"}}));
    }
        
    //unordered_map(first, last)
    {
        std::unordered_map<std::string, std::string> src({{"a", "abc"}, {"b", "def"}});
        run(cont_t(src.begin(), src.end()),
             cont_t::observed_t({{"a", "abc"}, {"b", "def"}}));
    }

    //unordered_map(il)
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
    
    //at fail
    {
        cont_t umap;
        bool ok{false};
        try{ umap.at("k1"); }
        catch(const std::out_of_range&)
        { ok = true; }
        BOOST_TEST(ok);
    }

    //at success
    {
        bool ok{true};
        cont_t umap{{"k1", "abc"}};
        try{ BOOST_TEST(umap.at("k1") == "abc"); }
        catch(const std::out_of_range&)
        { ok = false; }
        BOOST_TEST(ok);
    }

    //at const fail
    {
        cont_t umap;
        bool ok{false};
        try
        {
            const auto& cvalue = umap;
            cvalue.at("k1");
        }
        catch(const std::out_of_range&)
        { ok = true; }
        BOOST_TEST(ok);
    }

    //at const success
    {
        bool ok{true};
        cont_t umap{{"k1", "abc"}};
        try
        {
            const auto& cumap = umap;
            auto cvalue = cumap.at("k1");
            BOOST_TEST(cvalue == "abc");
        }
        catch(const std::out_of_range&)
        { ok = false; }
        BOOST_TEST(ok);            
    }
        
    //operator[lvalue] 
    {
        cont_t umap({{"k1", "abc"}});
        BOOST_TEST(umap["k1"] == "abc");
    }

    //operator[rvalue] 
    {
        cont_t umap({{"k1", "abc"}});
        umap.emplace("k1", "abc");
        BOOST_TEST(umap["k1"] == "abc");
    }

    //erase_if
    {
        using ounordered_map = coruja::unordered_map<int, int>;
        ounordered_map m{{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}};
        std::set<int> removed;
        auto c = m.before_erase([&](ounordered_map::value_type p){ removed.insert(p.first); });
        auto is_key_odd = [](ounordered_map::value_type p){ return p.first%2; };
        coruja::erase_if(m, is_key_odd);
        BOOST_TEST(removed == std::set<int>({1, 3, 5}));
        c.disconnect();
    }

    //erase
    {
        using ounordered_map = coruja::unordered_map<int, int>;
        ounordered_map m{{1, 1}, {2, 2}, {3, 3}, {5, 5}, {6, 6}};
        std::vector<int> removed;
        auto c = m.before_erase([&](ounordered_map::value_type p){ removed.push_back(p.first); });
        coruja::erase(m, ounordered_map::value_type{3, 3});
        BOOST_TEST(removed == std::vector<int>({3}));
        c.disconnect();
    }
}
