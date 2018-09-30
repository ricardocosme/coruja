#pragma once

#include "run_container.hpp"
#include "coruja/support/signal/any_connection.hpp"

using namespace coruja;

template<typename C>
void test_sequence_container()
{
    static_assert(std::is_same<typename C::value_type,
                  typename C::observed_t::value_type>::value, "");
    static_assert(std::is_same<typename C::reference,
                  typename C::observed_t::reference>::value, "");
    static_assert(std::is_same<typename C::const_reference,
                  typename C::observed_t::const_reference>::value, "");
    static_assert(std::is_same<typename C::iterator,
                  typename C::observed_t::iterator>::value, "");
    static_assert(std::is_same<typename C::const_iterator,
                  typename C::observed_t::const_iterator>::value, "");
    static_assert(std::is_same<typename C::difference_type,
                  typename C::observed_t::difference_type>::value, "");
    static_assert(std::is_same<typename C::size_type,
                  typename C::observed_t::size_type>::value, "");

    static_assert(is_observable_erasable_range<C>::value, "");
    
    using observed_t = typename C::observed_t;
    static_assert(std::is_same<typename C::reverse_iterator,
                  typename observed_t::reverse_iterator>::value, "");
    static_assert(std::is_same<typename C::const_reverse_iterator,
                  typename observed_t::const_reverse_iterator>::value, "");
    
    //default ctor
    {
        C cont;
    }
    
    //swap()
    {
        run(C({"something", "something"}), {"something", "something", "abc", "def", "ghi"},
            [](C& cont){
                C other{"abc", "def", "ghi"};
                cont.swap(other);
                return std::move(cont);
            },
            {"abc", "def", "ghi"});
    }
    
    //begin()
    {
        C cont;
        emplace(cont, "abc");
        BOOST_TEST(*cont.begin() == "abc");
        const auto& ccont = cont;
        BOOST_TEST(*ccont.begin() == "abc");
    }

    //cbegin()
    {
        C cont;
        emplace(cont, "abc");
        BOOST_TEST(*cont.cbegin() == "abc");
    }
    
    //end()
    {
        C cont;
        emplace(cont, "abc");
        emplace(cont, "def");
        BOOST_TEST(std::distance(cont.begin(), cont.end()) == 2);
        const auto& ccont = cont;
        BOOST_TEST(std::distance(ccont.begin(), ccont.end()) == 2);
    }

    //cend()
    {
        C cont;
        emplace(cont, "abc");
        emplace(cont, "def");
        BOOST_TEST(std::distance(cont.cbegin(), cont.cend()) == 2);
    }    

    //size()
    {
        C cont;
        emplace(cont, "abc");
        emplace(cont, "def");
        BOOST_TEST(cont.size() == 2);
    }
    
    //max_size()
    {
        C cont;
        emplace(cont, "abc");
        emplace(cont, "def");
        BOOST_TEST(cont.max_size() >= 2);
    }

    //empty()
    {
        C cont;
        BOOST_TEST(cont.empty());
        emplace(cont, "something");
        BOOST_TEST(!cont.empty());        
    }
        
    //clear() before_erase
    {
        C cont{"abc", "def"};
        bool before_called{false};
        scoped_any_connection c = cont.before_erase
            ([&before_called](typename C::value_type)
             { before_called = true; });
        cont.clear();
        BOOST_TEST(before_called);
        BOOST_TEST(cont.empty());
    }
                
    //erase() 
    {
        C cont{"abc", "def"};
        bool before_called{false};
        scoped_any_connection c = cont.before_erase
            ([&before_called](typename C::value_type e)
             {
                 BOOST_TEST(e == "abc");
                 before_called = true;
             });
        auto ret = cont.erase(cont.cbegin());
        BOOST_TEST(*ret == "def");
        BOOST_TEST(cont.size() == 1);
        BOOST_TEST(before_called);
    }
        
    //erase(first, last) 
    {
        C cont{"abc", "def"};
        bool before_called{false};
        scoped_any_connection c = cont.before_erase
            ([&before_called](typename C::value_type)
             { before_called = true; });
        auto ret = cont.erase(cont.cbegin(), cont.cend());
        BOOST_TEST(ret == cont.end());
        BOOST_TEST(cont.empty());
        BOOST_TEST(before_called);
    }
    
    //assign(n, val)
    {
        C cont;
        run(std::move(cont), {"something", "something", "something"},
            [](C& cont){
                cont.assign(3, "something");
                return std::move(cont);
            });
    }
    
    //assign(first, last)
    {
        C cont;
        run(std::move(cont), {"something", "something", "something"},
            [](C& cont){
                std::initializer_list<std::string> src
                    {"something", "something", "something"};
                cont.assign(src.begin(), src.end());
                return std::move(cont);
            });
    }
    
    //assign(initializer_list)
    {
        C cont;
        run(std::move(cont), {"something", "something", "something"},
            [](C& cont){
                std::initializer_list<std::string> src
                    {"something", "something", "something"};
                cont.assign(src);
                return std::move(cont);
            });
    }
    
    //rbegin()
    {
        C cont;
        cont.emplace_back("abc");
        cont.emplace_back("def");
        BOOST_TEST(*cont.rbegin() == "def");
        const auto& ccont = cont;
        BOOST_TEST(*ccont.rbegin() == "def");
    }
    
    //crbegin()
    {
        C cont;
        cont.emplace_back("abc");
        cont.emplace_back("def");
        BOOST_TEST(*cont.crbegin() == "def");
    }
    
    //rend()
    {
        C cont;
        cont.emplace_back("abc");
        cont.emplace_back("def");
        BOOST_TEST(*std::prev(cont.rend()) == "abc");
        const auto& ccont = cont;
        BOOST_TEST(*std::prev(ccont.rend()) == "abc");
    }
    
    //crend()
    {
        C cont;
        cont.emplace_back("abc");
        cont.emplace_back("def");
        BOOST_TEST(*std::prev(cont.crend()) == "abc");
    }

    //resize(n)
    {
        C cont{"abc", "def", "ghi"};
        bool before_erase_called{false};
        scoped_any_connection c = cont.before_erase
            ([&before_erase_called](typename C::value_type e)
             {
                 BOOST_TEST(e == "ghi");
                 before_erase_called = true;
             });
        cont.resize(2);
        BOOST_TEST(before_erase_called);
    }
    
    //resize(n)
    {
        run(C({"abc", "def", "ghi"}), {"abc", "def", "ghi", ""},
            [](C& cont){
                cont.resize(4);
                return std::move(cont);
            });
    }
    
    //resize(n, v)
    {
        C cont{"abc", "def", "ghi"};
        run(std::move(cont), {"abc", "def", "ghi"},
            [](C& cont){
                cont.resize(2, "something");
                return std::move(cont);
            },
            {"abc", "def"});
    }
    
    //resize(n, v)
    {
        C cont{"abc", "def", "ghi"};
        run(std::move(cont), {"abc", "def", "ghi", "something"},
            [](C& cont){
                cont.resize(4, "something");
                return std::move(cont);
            },
            {"abc", "def", "ghi", "something"});
    }
    
    //front()
    {
        C cont;
        cont.emplace(cont.cbegin(), "abc");
        BOOST_TEST(cont.front() == "abc");
    }
    
    //front() const
    {
        C cont;
        cont.emplace(cont.cbegin(), "abc");
        auto& ccont = cont;
        BOOST_TEST(ccont.front() == "abc");
    }
    
    //back()
    {
        C cont;
        cont.emplace_back("abc");
        cont.emplace_back("def");
        BOOST_TEST(cont.back() == "def");
    }
    
    //back() const
    {
        C cont;
        cont.emplace_back("abc");
        cont.emplace_back("def");
        const auto& ccont = cont;
        BOOST_TEST(ccont.back() == "def");
    }
    
    //emplace()
    {
        run(C{"abc"}, {"abc", "def"},
            [](C& cont)
            {
                cont.emplace(cont.cbegin(), "def");
                BOOST_TEST(cont.front() == "def");
                BOOST_TEST(cont.size() == 2);
                return std::move(cont);
            },
            {"def", "abc"});
    }
    
    //insert(pos, const value_type&)
    {
        run(C{"abc"}, {"abc", "def"},
            [](C& cont)
            {
                std::string v2 = "def";
                cont.insert(std::next(cont.cbegin()), v2);
                BOOST_TEST(cont.front() == "abc");
                BOOST_TEST(*std::next(cont.cbegin()) == "def");
                BOOST_TEST(cont.size() == 2);
                return std::move(cont);
            },
            {"abc", "def"});
    }

    //insert(pos, value_type&&)
    {
        run(C{"abc"}, {"abc", "def"},
            [](C& cont)
            {
                cont.insert(std::next(cont.cbegin()), "def");
                BOOST_TEST(cont.front() == "abc");
                BOOST_TEST(*std::next(cont.cbegin()) == "def");
                BOOST_TEST(cont.size() == 2);
                return std::move(cont);
            },
            {"abc", "def"});
    }
    
    //insert(pos, first, last)
    {
        run(C{"abc"}, {"abc", "abc", "def"},
            [](C& cont)
            {
                auto cit = cont.cbegin();
                std::initializer_list<std::string> src{"abc", "def"};
                cont.insert(cit, src.begin(), src.end());
                BOOST_TEST(*std::next(cont.cbegin()) == "def");
                BOOST_TEST(cont.size() == 3);
                return std::move(cont);
            },
            {"abc", "def", "abc"});
    }
    
    //insert(ilist)
    {
        run(C{"abc"}, {"abc", "abc", "def"},
            [](C& cont)
            {
                auto cit = cont.cbegin();
                cont.insert(cit, {"abc", "def"});
                BOOST_TEST(*std::next(cont.cbegin()) == "def");
                BOOST_TEST(cont.size() == 3);
                return std::move(cont);
            },
            {"abc", "def", "abc"});
    }
    
    //push_back()
    {
        run(C{"abc"}, {"abc", "def", "def"},
            [](C& cont)
            {
                cont.push_back(std::string{"def"});
                auto lvalue = std::string{"def"};
                cont.push_back(lvalue);
                BOOST_TEST(cont.front() == "abc");
                BOOST_TEST(*std::next(cont.begin()) == "def");
                BOOST_TEST(cont.back() == "def");
                return std::move(cont);
            },
            {"abc", "def", "def"});
    }
    
    //emplace_back()
    {
        run(C{"abc"}, {"abc", "def"},
            [](C& cont)
            {
                cont.emplace_back(std::string{"def"});
                BOOST_TEST(cont.front() == "abc");
                BOOST_TEST(cont.back() == "def");
                return std::move(cont);
            },
            {"abc", "def"});
    }
    
    //pop_back() before_erase
    {
        run(C{"abc", "def"}, {"abc", "def"},
            [](C& cont)
            {
                cont.pop_back();
                BOOST_TEST(cont.size() == 1);
                return std::move(cont);
            },
            {"abc"},
            {"def"});
    }

    //operator ==,!=, <, >, <=, >=
    {
        C lhs({"abc", "def"});
        C rhs({"abc", "def"});
        BOOST_TEST(lhs == rhs);
        BOOST_TEST(lhs == rhs.observed());
        BOOST_TEST(lhs.observed() == rhs);
        
        C ineq({"xxx", "xxx"});
        BOOST_TEST(lhs != ineq);
        BOOST_TEST(lhs != ineq.observed());
        BOOST_TEST(lhs.observed() != ineq);
    }
}
