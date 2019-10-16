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

    //splice observable and observed - empty other
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
    
    //splice observable and observed - other as rvalue - limited range
    {
        slist lst({"abc", "def", "ghi"});
        slist::observed_t other({"jkl", "mno"});
        std::vector<slist::value_type> after;
        lst.for_each([&](slist::value_type o)
                    { after.push_back(o); });
        lst.splice(lst.end(), other, other.begin(), std::next(other.begin()));
        BOOST_TEST((after == std::vector<std::string>
            {"abc", "def", "ghi", "jkl"}));
        BOOST_TEST(lst.size() == 4);
        BOOST_TEST(lst.back() == "jkl");
    }

    //splice observable and observed, other as a rvalue
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
    
    //erase_if
    {
        using olist = coruja::list<int>;
        olist v{{1, 2, 3, 4, 5, 6}};
        std::vector<int> removed;
        auto c = v.before_erase([&](int i){ removed.push_back(i); });
        auto is_odd = [](int i){ return i%2; };
        coruja::erase_if(v, is_odd);
        BOOST_TEST(removed == std::vector<int>({1, 3, 5}));
        c.disconnect();
    }

    //erase
    {
        slist vec({"xxx", "def", "xxx", "jkl", "xxx"});
        std::vector<std::string> removed;
        auto c = vec.before_erase([&](std::string s){ removed.push_back(s); });
        coruja::erase(vec, "xxx");
        BOOST_TEST(removed == std::vector<std::string>({"xxx", "xxx", "xxx"}));
        c.disconnect();
    }

    struct React2Splice
    {
        React2Splice(std::initializer_list<std::string> dst_list_, 
                     std::initializer_list<std::string> src_list_)
            :dst_list(dst_list_)
            ,src_list(src_list_)
        {
            dst_list.for_each([&](slist::value_type o)
                        { dst_list_inserted.push_back(o); });
            src_list.before_erase([&](slist::value_type o)
                        { src_list_removed.push_back(o); });
        }

        using list_type = std::vector<slist::value_type>;
        list_type dst_list_inserted, src_list_removed;

        slist dst_list, src_list;
    };

    //splice observables - dst_list is empty
    {
        React2Splice react{{}, {"123", "456", "789"}};

        react.dst_list.splice(react.dst_list.end(), react.src_list);

        BOOST_TEST((react.dst_list_inserted == React2Splice::list_type{"123", "456", "789"}));
        BOOST_TEST(react.dst_list.size() == 3);
        BOOST_TEST(react.dst_list.back() == "789");
        BOOST_TEST((react.src_list_removed == React2Splice::list_type{"123", "456", "789"}));
        BOOST_TEST(react.src_list_removed.size() == 3);
    }

    //splice observables - src_list is empty
    {
        React2Splice react{{"abc", "def", "ghi"}, {}};

        react.dst_list.splice(react.dst_list.end(), react.src_list);

        BOOST_TEST((react.dst_list_inserted == React2Splice::list_type{"abc", "def", "ghi"}));
        BOOST_TEST(react.dst_list.size() == 3);
        BOOST_TEST(react.dst_list.back() == "ghi");
        BOOST_TEST((react.src_list_removed == React2Splice::list_type{}));
        BOOST_TEST(react.src_list_removed.size() == 0);
    }

    //splice observables - limited range 
    {
        React2Splice react{{"abc", "def", "ghi"}, {"123", "456", "789"}};

        react.dst_list.splice(std::prev(react.dst_list.end()), react.src_list, 
                        std::next(react.src_list.begin()), std::prev(react.src_list.end()));

        BOOST_TEST((react.dst_list == slist{"abc", "def", "456", "ghi"}));
        BOOST_TEST(react.dst_list.size() == 4);
        BOOST_TEST(react.dst_list.back() == "ghi");
        BOOST_TEST((react.dst_list_inserted == React2Splice::list_type{"abc", "def", "ghi", "456"}));
        BOOST_TEST((react.src_list_removed == React2Splice::list_type{"456"}));
        BOOST_TEST(react.src_list_removed.size() == 1);
    }

    //splice observables - single element 
    {
        React2Splice react{{"abc", "def", "ghi"}, {"123"}};

        react.dst_list.splice(std::prev(react.dst_list.end()), react.src_list, 
                              react.src_list.begin());

        BOOST_TEST((react.dst_list == slist{"abc", "def", "123", "ghi"}));
        BOOST_TEST((react.dst_list_inserted == React2Splice::list_type{"abc", "def", "ghi", "123"}));
        BOOST_TEST(react.dst_list_inserted.size() == 4);
        BOOST_TEST(react.dst_list_inserted.back() == "123");
        BOOST_TEST((react.src_list_removed == React2Splice::list_type{"123"}));
        BOOST_TEST(react.src_list_removed.size() == 1);
    }

    //splice observables - duplicated iterator 
    {
        React2Splice react{{"abc"}, {"123"}};

        react.dst_list.splice(react.dst_list.end(), react.src_list, 
                              react.src_list.begin(), react.src_list.begin());

        BOOST_TEST((react.dst_list_inserted == React2Splice::list_type{"abc"}));
        BOOST_TEST(react.dst_list.size() == 1);
        BOOST_TEST(react.dst_list.back() == "abc");
        BOOST_TEST((react.src_list_removed == React2Splice::list_type{}));
        BOOST_TEST(react.src_list_removed.size() == 0);
    }

    //splice observables - other as rvalue
    {
        React2Splice react{{"abc"}, {}};

        react.dst_list.splice(react.dst_list.end(), slist{"123"});

        BOOST_TEST((react.dst_list_inserted == React2Splice::list_type{"abc", "123"}));
        BOOST_TEST(react.dst_list.size() == 2);
        BOOST_TEST(react.dst_list.back() == "123");
    }
}
