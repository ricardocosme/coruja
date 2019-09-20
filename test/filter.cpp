
#include "check_equal.hpp"

#include <coruja/container/view/filter.hpp>
#include <coruja/container/vector.hpp>
#include <range/v3/view/reverse.hpp>

#include <string>

using namespace coruja;

using svector = vector<std::string>;
using stdvector = std::vector<std::string>;
struct True{
    bool operator()(std::string v) { return true; }
};

struct Copy{
    void operator()(std::string v) { vec.push_back(v); }
    stdvector& vec;
};

int main()
{
    {
        svector vector;
        auto filter = view::filter(vector, True{});
        (void)filter;
    }

    // empty
    {
        svector vector;
        auto filter = view::filter(vector, True{});
        stdvector resObser;
        stdvector resRng;
        stdvector resRngRev;
        filter.for_each(Copy{resObser});
        for(auto&& v: filter)
        { resRng.push_back(v); }
        for(auto&& v: filter | ranges::view::reverse)
        { resRngRev.push_back(v); }

        BOOST_TEST( ranges::size(vector) == ranges::size(resObser) );
        BOOST_TEST( ranges::size(vector) == ranges::size(resRng) );
        BOOST_TEST( ranges::size(vector) == ranges::size(resRngRev) );
    }

    // copy all
    {
        svector vector = { "Hello", "World", "Hello", "World", "world" };
        auto filter = view::filter(vector, True{});
        stdvector resObser;
        stdvector resRng;
        stdvector resRngRev;
        filter.for_each(Copy{resObser});
        for(auto&& v: filter)
        { resRng.push_back(v); }
        for(auto&& v: filter | ranges::view::reverse)
        { resRngRev.push_back(v); }

        check_equal(resObser, { "Hello", "World", "Hello", "World", "world" });
        check_equal(resRng,  { "Hello", "World", "Hello", "World", "world" });
        check_equal(resRngRev,  { "world", "World", "Hello", "World", "Hello"});

    }

    // only World
    {
        svector vector = { "Hello", "World", "hello", "World", "world" };
        auto filter = view::filter(vector, [](std::string v)
        { return v == "World"; });
        stdvector resObser;
        stdvector resRng;
        stdvector resRngRev;
        filter.for_each(Copy{resObser});
        for(auto&& v: filter)
        { resRng.push_back(v); }
        for(auto&& v: filter | ranges::view::reverse)
        { resRngRev.push_back(v); }

        check_equal(resObser, { "World", "World" });
        check_equal(resRng, { "World", "World" });
        check_equal(resRngRev, { "World", "World" });
    }

    // copy none
    {
        svector vector = { "Hello", "World", "Hello", "World", "world" };
        auto filter = view::filter(vector, [](std::string v)
        { return v == "orld"; });

        stdvector resObser;
        stdvector resRng;
        stdvector resRngRev;
        filter.for_each(Copy{resObser});
        for(auto&& v: filter)
        { resRng.push_back(v); }
        for(auto&& v: filter)
        { resRngRev.push_back(v); }

        BOOST_TEST( ranges::size(resObser) == 0 );
        BOOST_TEST( ranges::size(resRng) == 0 );
        BOOST_TEST( ranges::size(resRngRev) == 0 );
    }

    // erase no match
    {
        svector vector = { "Hello", "World", "Hello", "World", "world" };
        auto filter = view::filter(vector, [](std::string v)
        { return v == "World"; });

        stdvector resObser;
        filter.before_erase(Copy{resObser});
        vector.erase(vector.begin() + 4);
        vector.erase(vector.begin() + 2);
        vector.erase(vector.begin() + 0);

        BOOST_TEST( ranges::size(resObser) == 0 );
    }

    // erase match
    {
        svector vector = { "Hello", "World", "Hello", "World", "world" };
        auto filter = view::filter(vector, [](std::string v)
        { return v == "World"; });

        stdvector resObser;
        filter.before_erase(Copy{resObser});
        vector.erase(vector.begin() + 3);
        vector.erase(vector.begin() + 1);

        check_equal(resObser, { "World", "World"});
    }

    // erase all
    {
        svector vector = { "Hello", "World", "Hello", "World", "world" };
        auto filter = view::filter(vector, [](std::string v)
        { return v == "World"; });

        stdvector resObser;
        filter.before_erase(Copy{resObser});
        vector.clear();

        check_equal(resObser, { "World", "World"});
    }
}
