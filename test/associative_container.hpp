#pragma once

#include "run_container.hpp"
#include "coruja/support/signal/any_connection.hpp"

using namespace coruja;

template<typename T, typename E = void>
struct is_map : std::false_type {};

template<typename T>
struct is_map<T, void_t<typename T::mapped_type>> : std::true_type{};

template<typename C>
inline typename std::enable_if<!is_map<C>::value, typename C::observed_t>::type
exp(std::initializer_list<std::pair<const std::string, std::string>> il)
{
    typename C::observed_t exp;
    for(auto&& e : il)
        exp.emplace(e.first);
    return exp;
}

template<typename C>
inline typename std::enable_if<is_map<C>::value, typename C::observed_t>::type
exp(std::initializer_list<std::pair<const std::string, std::string>> il)
{ return {il}; }

template<typename C, typename A1, typename A2>
inline typename std::enable_if<!is_map<C>::value>::type
emplace(C& c, A1&& a1, A2&&)
{ c.emplace(std::forward<A1>(a1)); }

template<typename C, typename... Args>
inline typename std::enable_if<is_map<C>::value>::type
emplace(C& c, Args&&... args)
{ c.emplace(std::forward<Args>(args)...); }

template<typename C, typename A1, typename A2>
inline typename std::enable_if<!is_map<C>::value>::type
emplace_hint(C& c, typename C::iterator hint, A1&& a1, A2&&)
{ c.emplace_hint(hint, std::forward<A1>(a1)); }

template<typename C, typename... Args>
inline typename std::enable_if<is_map<C>::value>::type
emplace_hint(C& c, typename C::iterator hint, Args&&... args)
{ c.emplace_hint(hint, std::forward<Args>(args)...); }

template<typename C, typename A1, typename A2>
inline typename std::enable_if<!is_map<C>::value, typename C::value_type>::type
value_type(A1&& a1, A2&&)
{ return {std::forward<A1>(a1)}; }

template<typename C, typename A1, typename A2>
inline typename std::enable_if<is_map<C>::value, typename C::value_type>::type
value_type(A1&& a1, A2&& a2)
{ return {std::forward<A1>(a1), std::forward<A2>(a2)}; }

template<typename C>
void test_associative_container()
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
        
    //default ctor
    {
        C cont;
    }
    
    //swap()
    {
        run(C(exp<C>({{"k1", "something"}, {"k2", "something"}})),
            exp<C>({{"k1", "something"}, {"k2", "something"}, {"k3", "abc"}, {"k4", "def"}, {"k5", "ghi"}}),
            [](C& cont){
                C other(exp<C>({{"k3", "abc"}, {"k4", "def"}, {"k5", "ghi"}}));
                cont.swap(other);
                return std::move(cont);
            },
            exp<C>({{"k3", "abc"}, {"k4", "def"}, {"k5", "ghi"}}));
    }
    
    //begin()
    {
        C cont;
        emplace(cont, "k1", "abc");
        BOOST_TEST(*cont.begin() == value_type<C>("k1", "abc"));
        const auto& ccont = cont;
        BOOST_TEST(*ccont.begin() == value_type<C>("k1", "abc"));
    }

    //cbegin()
    {
        C cont;
        emplace(cont, "k1", "abc");
        BOOST_TEST(*cont.cbegin() == value_type<C>("k1", "abc"));
    }
    
    //end()
    {
        C cont;
        emplace(cont, "k1", "abc");
        emplace(cont, "k2", "def");
        BOOST_TEST(std::distance(cont.begin(), cont.end()) == 2);
        const auto& ccont = cont;
        BOOST_TEST(std::distance(ccont.begin(), ccont.end()) == 2);
    }

    //cend()
    {
        C cont;
        emplace(cont, "k1", "abc");
        emplace(cont, "k2", "def");
        BOOST_TEST(std::distance(cont.cbegin(), cont.cend()) == 2);
    }    

    //size()
    {
        C cont;
        emplace(cont, "k1", "abc");
        emplace(cont, "k2", "def");
        BOOST_TEST(cont.size() == 2);
    }
    
    //max_size()
    {
        C cont;
        emplace(cont, "k1", "abc");
        emplace(cont, "k2", "def");
        BOOST_TEST(cont.max_size() >= 2);
    }

    //empty()
    {
        C cont;
        BOOST_TEST(cont.empty());
        emplace(cont, "k", "something");
        BOOST_TEST(!cont.empty());        
    }
        
    //clear() before_erase
    {
        C cont(exp<C>({{"k1", "abc"}, {"k2", "def"}}));
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
        C cont(exp<C>({{"k1", "abc"}, {"k2", "def"}}));
        bool before_called{false};
        scoped_any_connection c = cont.before_erase
            ([&before_called](typename C::value_type e)
             {
                 BOOST_TEST(e == value_type<C>("k1", "abc"));
                 before_called = true;
             });
        auto ret = cont.erase(cont.find("k1"));
        BOOST_TEST(cont.size() == 1);
        BOOST_TEST(before_called);
    }
        
    //erase(first, last) 
    {
        C cont(exp<C>({{"k1", "abc"}, {"k2", "def"}}));
        bool before_called{false};
        scoped_any_connection c = cont.before_erase
            ([&before_called](typename C::value_type)
             { before_called = true; });
        auto ret = cont.erase(cont.cbegin(), cont.cend());
        BOOST_TEST(ret == cont.end());
        BOOST_TEST(cont.empty());
        BOOST_TEST(before_called);
    }
    
    //count
    {
        C cont(exp<C>({{"k1", "abc"}, {"k2", "def"}}));
        emplace(cont, "k1", "abc");
        assert(cont.count("k1") == 1);
        assert(cont.count("k0") == 0);
    }

    //erase(key) 
    {
        C cont(exp<C>({{"k1", "abc"}, {"k2", "def"}}));
        bool before_called{false};
        scoped_any_connection c = cont.before_erase
            ([&before_called](typename C::value_type e)
             {
                 BOOST_TEST(e == value_type<C>("k1", "abc"));
                 before_called = true;
             });
        auto n = cont.erase("k1");
        BOOST_TEST(n == 1);
        BOOST_TEST(before_called);
    }
    
    //emplace()
    {
        run(C(exp<C>({{"k1", "abc"}})),
            exp<C>({{"k1" , "abc"}, {"k2", "def"}}),
            [](C& cont)
            {
                emplace(cont, "k2", "def");
                BOOST_TEST(cont.size() == 2);
                return std::move(cont);
            },
            exp<C>({{"k1" , "abc"}, {"k2", "def"}}));
    }

    //emplace_hint()
    {
        run(C(exp<C>({{"k1", "abc"}})),
            exp<C>({{"k1" , "abc"}, {"k2", "def"}}),
            [](C& cont)
            {
                emplace_hint(cont, cont.end(), "k2", "def");
                BOOST_TEST(cont.size() == 2);
                return std::move(cont);
            },
            exp<C>({{"k1" , "abc"}, {"k2", "def"}}));
    }

    //insert(const value_type&)
    {
        run(C(exp<C>({{"k1", "abc"}})),
            exp<C>({{"k1" , "abc"}, {"k2", "def"}}),
            [](C& cont)
            {
                auto v2 = value_type<C>("k2", "def");
                auto it = cont.insert(v2);
                BOOST_TEST(it.second);
                BOOST_TEST(*it.first == value_type<C>("k2", "def"));
                BOOST_TEST(cont.size() == 2);
                return std::move(cont);
            },
            exp<C>({{"k1" , "abc"}, {"k2", "def"}}));
    }

    //insert(value_type&&)
    {
        run(C(exp<C>({{"k1", "abc"}})),
            exp<C>({{"k1" , "abc"}, {"k2", "def"}}),
            [](C& cont)
            {
                auto it = cont.insert(value_type<C>("k2", "def"));
                BOOST_TEST(it.second);
                BOOST_TEST(*it.first == value_type<C>("k2", "def"));
                BOOST_TEST(cont.size() == 2);
                return std::move(cont);
            },
            exp<C>({{"k1" , "abc"}, {"k2", "def"}}));
    }

    //insert(hint, const value_type&)
    {
        run(C(exp<C>({{"k1", "abc"}})),
            exp<C>({{"k1" , "abc"}, {"k2", "def"}}),
            [](C& cont)
            {
                auto v2 = value_type<C>("k2", "def");
                auto it = cont.insert(cont.end(), v2);
                BOOST_TEST(*it == value_type<C>("k2", "def"));
                BOOST_TEST(cont.size() == 2);
                return std::move(cont);
            },
            exp<C>({{"k1" , "abc"}, {"k2", "def"}}));
    }

    //insert(hint, value_type&&)
    {
        run(C(exp<C>({{"k1", "abc"}})),
            exp<C>({{"k1" , "abc"}, {"k2", "def"}}),
            [](C& cont)
            {
                auto it = cont.insert(cont.end(), value_type<C>("k2", "def"));
                BOOST_TEST(*it == value_type<C>("k2", "def"));
                BOOST_TEST(cont.size() == 2);
                return std::move(cont);
            },
            exp<C>({{"k1" , "abc"}, {"k2", "def"}}));
    }
    
    //find fail
    {
        C cont;
        BOOST_TEST(cont.find("k1") == cont.end());
        const auto& ccont = cont;
        BOOST_TEST(ccont.find("k1") == cont.cend());
        
    }
    
    //find success
    {
        C cont(exp<C>({{"k1" , "abc"}, {"k2", "def"}}));
        auto it = cont.find("k2");
        BOOST_TEST(it != cont.end());
    }
    
    //operator ==,!=
    {
        C lhs(exp<C>({{"k1", "abc"}, {"k2", "def"}}));
        C rhs(exp<C>({{"k1", "abc"}, {"k2", "def"}}));
        BOOST_TEST(lhs == rhs);
        BOOST_TEST(lhs == rhs.observed());
        BOOST_TEST(lhs.observed() == rhs);
        C ineq(exp<C>({{"a", "xxx"}, {"b", "xxx"}}));
        BOOST_TEST(lhs != ineq);
        BOOST_TEST(lhs != ineq.observed());
        BOOST_TEST(lhs.observed() != ineq);
    }
}
