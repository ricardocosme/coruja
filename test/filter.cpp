
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

struct Copy_by_It{
    template<typename From, typename It>
    void operator()(From& from, It it) { vec.push_back( *it ); }
    stdvector& vec;
};

template<typename OER, typename Out>
void copy(OER& obsRng, Out& resObs, Out& resObs_it, Out& resRng, Out& resRng_rev)
{
    obsRng.for_each(Copy{resObs});
    obsRng.for_each(Copy_by_It{resObs_it});
    for(auto&& v: obsRng)
    { resRng.push_back(v); }
    for(auto&& v: obsRng | ranges::view::reverse)
    { resRng_rev.push_back(v); }
}

int main()
{
    {
        svector vector;
        auto filter = view::filter(vector, True{});
        (void)filter;
    }

    //Concept
    {
        svector s;
        auto t = view::filter(s, True{});
        using fsvector = decltype(t);
        static_assert(std::is_same<fsvector::observed_t,
                      svector::observed_t>::value, "");
        static_assert(std::is_same<fsvector::for_each_connection_t,
                      svector::for_each_connection_t>::value, "");
        static_assert(std::is_same<fsvector::before_erase_connection_t,
                      svector::before_erase_connection_t>::value, "");

        static_assert(is_observable_erasable_range<svector>::value, "");
        static_assert(is_observable_erasable_range<fsvector>::value, "");
    }

    // empty
    {
        svector vector;
        auto filter = view::filter(vector, True{});
        stdvector resObser;
        stdvector resObserIt;
        stdvector resRng;
        stdvector resRngRev;
        copy(filter, resObser, resObserIt, resRng, resRngRev);

        BOOST_TEST( ranges::size(vector) == ranges::size(resObser) );
        BOOST_TEST( ranges::size(vector) == ranges::size(resObserIt) );
        BOOST_TEST( ranges::size(vector) == ranges::size(resRng) );
        BOOST_TEST( ranges::size(vector) == ranges::size(resRngRev) );
    }

    // copy all
    {
        svector vector = { "Hello", "World", "Hello", "World", "world" };
        auto filter = view::filter(vector, True{});
        stdvector resObser;
        stdvector resObserIt;
        stdvector resRng;
        stdvector resRngRev;
        copy(filter, resObser, resObserIt, resRng, resRngRev);

        check_equal(resObser, { "Hello", "World", "Hello", "World", "world" });
        check_equal(resObserIt, { "Hello", "World", "Hello", "World", "world" });
        check_equal(resRng,  { "Hello", "World", "Hello", "World", "world" });
        check_equal(resRngRev,  { "world", "World", "Hello", "World", "Hello"});

    }

    // only World
    {
        svector vector = { "Hello", "World", "hello", "World", "world" };
        auto filter = view::filter(vector, [](std::string v)
        { return v == "World"; });
        stdvector resObser;
        stdvector resObserIt;
        stdvector resRng;
        stdvector resRngRev;
        copy(filter, resObser, resObserIt, resRng, resRngRev);

        check_equal(resObser, { "World", "World" });
        check_equal(resObserIt, { "World", "World" });
        check_equal(resRng, { "World", "World" });
        check_equal(resRngRev, { "World", "World" });
    }

    // copy none
    {
        svector vector = { "Hello", "World", "Hello", "World", "world" };
        auto filter = view::filter(vector, [](std::string v)
        { return v == "orld"; });

        stdvector resObser;
        stdvector resObserIt;
        stdvector resRng;
        stdvector resRngRev;
        copy(filter, resObser, resObserIt, resRng, resRngRev);

        BOOST_TEST( ranges::size(resObser) == 0 );
        BOOST_TEST( ranges::size(resObserIt) == 0 );
        BOOST_TEST( ranges::size(resRng) == 0 );
        BOOST_TEST( ranges::size(resRngRev) == 0 );
    }

    // erase no match
    {
        svector vector = { "Hello", "World", "Hello", "World", "world" };
        auto filter = view::filter(vector, [](std::string v)
        { return v == "World"; });

        stdvector resObser;
        stdvector resObserIt;
        filter.before_erase(Copy{resObser});
        filter.before_erase(Copy_by_It{resObserIt});

        vector.erase(vector.begin() + 4);
        vector.erase(vector.begin() + 2);
        vector.erase(vector.begin() + 0);

        BOOST_TEST( ranges::size(resObser) == 0 );
        BOOST_TEST( ranges::size(resObserIt) == 0 );
    }

    // erase match
    {
        svector vector = { "Hello", "World", "Hello", "World", "world" };
        auto filter = view::filter(vector, [](std::string v)
        { return v == "World"; });

        stdvector resObser;
        stdvector resObserIt;
        filter.before_erase(Copy{resObser});
        filter.before_erase(Copy_by_It{resObserIt});

        vector.erase(vector.begin() + 3);
        vector.erase(vector.begin() + 1);

        check_equal(resObser, { "World", "World"});
        check_equal(resObserIt, { "World", "World"});
    }

    // erase all
    {
        svector vector = { "Hello", "World", "Hello", "World", "world" };
        auto filter = view::filter(vector, [](std::string v)
        { return v == "World"; });

        stdvector resObser;
        stdvector resObserIt;
        filter.before_erase(Copy{resObser});
        filter.before_erase(Copy_by_It{resObserIt});

        vector.clear();

        check_equal(resObser, { "World", "World"});
        check_equal(resObserIt, { "World", "World"});
    }
}
