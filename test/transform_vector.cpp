#include "check_equal.hpp"

#include <coruja/container/transform.hpp>
#include <coruja/container/vector.hpp>

#include <boost/optional/optional.hpp>
#include <range/v3/back.hpp>
#include <range/v3/front.hpp>
#include <range/v3/range_concepts.hpp>
#include <string>

using namespace coruja;

using svector = vector<std::string>;

struct append_space {
    std::string operator()(const std::string& s)
    { return s + " "; }
};

struct append_exclamation {
    std::string operator()(const std::string& s)
    { return s + "!"; }
};

struct check_str {
    template<typename T>
    void operator()(T&& s) const
    {
        b = true;
        BOOST_TEST(s == expected);
    }
    bool& b;
    std::string& expected;
};

template<typename V, typename T>
void run(V& s, T&& t, std::string suffix)
{
    bool called{false};
    std::string fexpected{"hello" + suffix};
    t.for_each(check_str{called, fexpected});
    bool bcalled{false};
    std::string bexpected{"hello" + suffix};
    t.before_erase(check_str{bcalled, bexpected});
    BOOST_TEST(called);
    BOOST_TEST(ranges::front(t) == "hello" + suffix);
    fexpected = "jimmy" + suffix;
    called = false;
    s.emplace_back("jimmy");
    BOOST_TEST(called);
    BOOST_TEST(ranges::back(t) == "jimmy" + suffix);
    s.erase(s.begin());
    BOOST_TEST(bcalled);
    bcalled = false;
    bexpected = "jimmy" + suffix;
    s.erase(s.begin());
    BOOST_TEST(bcalled);
}

int main()
{
    {
        svector s;
        auto t = transform(s, [](std::string& s){ return s; });
        using tsvector = decltype(t);
        static_assert(std::is_same<tsvector::observed_t,
                      svector::observed_t>::value, "");
        static_assert(std::is_same<tsvector::for_each_connection_t,
                      svector::for_each_connection_t>::value, "");
        static_assert(std::is_same<tsvector::before_erase_connection_t,
                      svector::before_erase_connection_t>::value, "");

        static_assert(is_observable_erasable_range<svector>::value, "");
        static_assert(is_observable_erasable_range<tsvector>::value, "");
    }

    //default ctor
    {
        svector s;
        auto t = transform(s, [](std::string& s){ return s; });
        using tsvector = decltype(t);
        tsvector def;
    }

    //transform(lv, rv) rv is const
    {
        svector s{"hello"};
        auto t = transform(s, [](std::string& s){ return s + "!"; });
        run(s, t, "!");
    }

    //transform(lv, rv) rv is non const
    {
        svector s{"hello"};
        auto t = transform(s, append_exclamation{});
        run(s, t, "!");
    }
    
    //transform(lv, lv)
    {
        svector s{"hello"};
        append_exclamation f{};
        auto t = transform(s, f);
        run(s, t, "!");
    }
    
    //vector | transform | transform (using lv)
    {
        svector s{"hello"};
        auto t0 = transform(s, append_space{});
        auto t1 = transform(t0, append_exclamation{});
        run(s, t1, " !");
    }
    
    //vector | transform | transform (using rv)
    {
        svector s{"hello"};
        auto t = transform(transform(s, append_space{}),
                           append_exclamation{});
        run(s, t, " !");
    }
    
    //observed()
    {
        svector s{"hello"};
        auto t = transform(s, append_exclamation{});
        BOOST_TEST(t.observed().front() == "hello");
    }
    
    //copy ctor
    {
        svector s{"hello"};
        auto t = transform(s, append_exclamation{});
        auto ct = t;
        run(s, ct, "!");
    }
    
    //copy assignment operator
    {
        svector s{"hello"};
        auto t = transform(s, append_exclamation{});
        decltype(t) ct;
        ct = t;
        run(s, ct, "!");
    }

    //move ctor
    {
        svector s{"hello"};
        auto t = transform(s, append_exclamation{});
        auto ct = std::move(t);
        run(s, ct, "!");
    }
    
    //move assignment operator
    {
        svector s{"hello"};
        auto t = transform(s, append_exclamation{});
        decltype(t) ct;
        ct = std::move(t);
        run(s, ct, "!");
    }
}
