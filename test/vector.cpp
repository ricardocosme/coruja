#include "check_equal.hpp"
#include "sequence_container.hpp"

#include <boost/core/lightweight_test.hpp>
#include <coruja/container/vector.hpp>

#include <string>

using namespace coruja;

using svector = vector<std::string>;

int main()
{    
    static_assert(std::is_nothrow_move_constructible<svector>::value, "");
    static_assert(std::is_nothrow_move_assignable<svector>::value, "");

    test_sequence_container<svector>();
    
    //vector(observed_t) rv
    {
        svector vec(std::vector<std::string>{"abc", "def", "ghi"});
        run(std::move(vec), {"abc", "def", "ghi"});
    }
    
    //vector(observed_t) lv
    {
        std::vector<std::string> src{"abc", "def", "ghi"};
        svector vec(src);
        run(std::move(vec), {"abc", "def", "ghi"});
    }

    //vector(size_type)
    {
        svector vec(5);
        run(std::move(vec), {"", "", "", "", ""});
    }

    //vector(size_type, value)
    {
        svector vec(5, "something");
        run(std::move(vec),
            {"something", "something", "something", "something", "something"});
    }

    //vector(il)
    {
        svector vec{"abc", "def", "ghi"};
        BOOST_TEST(vec.size() == 3);
        run(std::move(vec), {"abc", "def", "ghi"});
    }

    //vector(first, last)
    {
        std::vector<std::string> src{"abc", "def", "ghi"};
        svector vec(src.begin(), src.end());
        run(std::move(vec), {"abc", "def", "ghi"});
    }
    
    //reserve()/capacity()
    {
        svector vec;
        vec.reserve(100);
        BOOST_TEST(vec.capacity() >= 100);
    }
    
    //shrink_to_fit()
    {
        svector vec;
        vec.reserve(100);
        vec.emplace_back("abc");
        vec.shrink_to_fit();
        BOOST_TEST(vec.capacity() < 100);
    }
    
    //at fail
    {
        svector vec;
        bool ok{false};
        try
        { vec.at(0); }
        catch(const std::out_of_range&)
        { ok = true; }
        BOOST_TEST(ok);
    }

    //at success
    {
        bool ok{true};
        svector vec;
        vec.emplace(vec.cbegin(), "abc");
        try
        { BOOST_TEST(vec.at(0) == "abc"); }
        catch(const std::out_of_range&)
        { ok = false; }
        BOOST_TEST(ok);
    }
    
    //at const fail
    {
        svector vec;
        bool ok{false};
        try
        {
            const auto& cvalue = vec;
            cvalue.at(0);
        }
        catch(const std::out_of_range&)
        { ok = true; }
        BOOST_TEST(ok);
    }

    //at const success
    {
        bool ok{true};
        svector vec;
        vec.emplace(vec.cbegin(), "abc");
        try
        {
            const auto& cvec = vec;
            auto cvalue = cvec.at(0);
            BOOST_TEST(cvalue == "abc");
        }
        catch(const std::out_of_range&)
        { ok = false; }
        BOOST_TEST(ok);            
    }

    //operator[] 
    {
        svector vec;
        vec.emplace(vec.cbegin(), "abc");
        BOOST_TEST(vec[0] == "abc");
    }

    //operator[] const
    {
        svector vec;
        vec.emplace(vec.cbegin(), "abc");
        const auto& cvec = vec;
        BOOST_TEST(cvec[0] == "abc");
    }
}
