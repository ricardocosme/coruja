#include "check_equal.hpp"
#include "sequence_container.hpp"

#include <boost/core/lightweight_test.hpp>
#include <coruja/container/list.hpp>

#include <string>

using namespace coruja;

using slist = list<std::string>;

int main()
{    
    test_sequence_container<slist>();
    
    //list(size_type, value)
    {
        run(slist(3, "abc"), {"abc", "abc", "abc"});
    }
    
    //list(size_type)
    {
        slist lst(3);
        BOOST_TEST(lst.size() == 3);
    }
    
    //list(first, last)
    {
        std::vector<std::string> src{"abc", "def", "ghi"};
        run(slist(src.begin(), src.end()), {"abc", "def", "ghi"});
    }
    
    //list(first, last)
    {
        run(slist({"abc", "def", "ghi"}), {"abc", "def", "ghi"});
    }
    
    //emplace_front()
    {
        slist cont;
        bool called{false};
        cont.emplace_front("abc");
        cont.for_each([&](slist::value_type)
                      { called = true; });
        cont.emplace_front("def");
        BOOST_TEST(called);
        BOOST_TEST(cont.front() == "def");
        BOOST_TEST(cont.back() == "abc");
    }

    //remove
    {
        slist lst({"abc", "def", "abc"});
        bool called{false};
        lst.before_erase([&](slist::value_type){ called = true; });
        lst.remove("abc");
        BOOST_TEST(called);
        BOOST_TEST(lst.size() == 1);
        BOOST_TEST(lst.front() == "def");
    }

    //splice - empty other
    {
        slist lst({"abc", "def", "ghi"});
        slist::observed_t other;
        std::vector<slist::value_type> after;
        lst.for_each([&](slist::value_type o)
                    { after.push_back(o); });
        lst.splice(lst.end(), other);
        BOOST_TEST((after == std::vector<std::string>
            {"abc", "def", "ghi"}));
        BOOST_TEST(lst.size() == 3);
        BOOST_TEST(lst.back() == "ghi");
    }
    
    //splice
    {
        slist lst({"abc", "def", "ghi"});
        slist::observed_t other({"jkl", "mno"});
        std::vector<slist::value_type> after;
        lst.for_each([&](slist::value_type o)
                    { after.push_back(o); });
        lst.splice(lst.end(), other);
        BOOST_TEST((after == std::vector<std::string>
            {"abc", "def", "ghi", "jkl", "mno"}));
        BOOST_TEST(lst.size() == 5);
        BOOST_TEST(lst.back() == "mno");
    }

    //splice, other as a rvalue
    {
        slist lst({"abc", "def", "ghi"});
        std::vector<slist::value_type> after;
        lst.for_each([&](slist::value_type o)
                    { after.push_back(o); });
        lst.splice(lst.end(), slist::observed_t{"jkl", "mno"});
        BOOST_TEST((after == std::vector<std::string>
            {"abc", "def", "ghi", "jkl", "mno"}));
        BOOST_TEST(lst.size() == 5);
        BOOST_TEST(lst.back() == "mno");
    }
}
