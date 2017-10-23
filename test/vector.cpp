#include "check_equal.hpp"

#include <coruja/container/vector.hpp>

#include <string>

using namespace coruja;

using svector = vector<std::string>;

int main()
{    
    static_assert(std::is_same<svector::observed_t,
                  std::vector<std::string>>::value, "");
    static_assert(std::is_same<svector::size_type,
                  std::vector<std::string>::size_type>::value, "");
    static_assert(std::is_same<svector::value_type,
                  std::vector<std::string>::value_type>::value, "");
    static_assert(std::is_same<svector::const_iterator,
                  std::vector<std::string>::const_iterator>::value, "");
    static_assert(std::is_same<svector::const_reverse_iterator,
                  std::vector<std::string>::const_reverse_iterator>::value, "");
    static_assert(std::is_same<svector::const_reference,
                  std::vector<std::string>::const_reference>::value, "");

    static_assert(std::is_nothrow_default_constructible<svector>::value, "");
    static_assert(std::is_nothrow_destructible<svector>::value, "");
    
    static_assert(std::is_nothrow_move_constructible<svector>::value, "");
    // static_assert(std::is_nothrow_move_assignable<svector>::value, "");

    static_assert(is_observable_erasable_range<svector>::value, "");
    
    //default ctor
    {
        svector vec;
    }

    //vector(vector&&)
    {
        svector vec{"abc", "def", "ghi"};
        bool called{false};
        vec.before_erase([&called](svector::value_type)
                         { called = true; });
        auto vec2(std::move(vec));
        check_equal(vec2, {"abc", "def", "ghi"});
        vec2.erase(vec2.cbegin());
        assert(called);
    }
    
    //operator=(vector&&)
    {
        svector vec{"abc", "def", "ghi"};
        bool called{false};
        vec.before_erase([&called](svector::value_type)
                         { called = true; });
        svector vec2;
        vec2 = std::move(vec);
        check_equal(vec2, {"abc", "def", "ghi"});
        vec2.erase(vec2.cbegin());
        assert(called);
    }
    
    {
        svector vec(5);
        assert(vec.size() == 5);
        for(auto e : vec)
            assert(e == std::string{});
    }
    
    {
        svector vec(5, "something");
        assert(vec.size() == 5);
        for(auto e : vec)
            assert(e == std::string{"something"});
    }
    
    {
        svector vec{"abc", "def", "ghi"};
        assert(vec.size() == 3);
        check_equal(vec, {"abc", "def", "ghi"});
    }
    
    {
        std::vector<std::string> src{"abc", "def", "ghi"};
        svector vec(src.begin(), src.end());
        assert(vec.size() == 3);
        check_equal(vec, {"abc", "def", "ghi"});
    }

    //assign(n, val)
    {
        svector vec;
        bool after_insert_called{false};
        bool after_insert_by_ref_called{false};
        vec.after_insert
            ([&after_insert_called](svector& cont, svector::iterator it)
             {
                 assert(*it == "something");
                 after_insert_called = true;
             });
        vec.after_insert
            ([&after_insert_by_ref_called](svector::value_type& v)
             {
                 assert(v == "something");
                 after_insert_by_ref_called = true;
             });
        vec.assign(3, "something");
        assert(after_insert_called);
        assert(after_insert_by_ref_called);
        check_equal(vec, {"something", "something", "something"});
    }
    
    //assign(first, last)
    {
        svector vec;
        bool after_insert_called{false};
        bool after_insert_by_ref_called{false};
        vec.after_insert
            ([&after_insert_called](svector& cont, svector::iterator it)
             {
                 assert(*it == "something");
                 after_insert_called = true;
             });
        vec.after_insert
            ([&after_insert_by_ref_called](svector::value_type& v)
             {
                 assert(v == "something");
                 after_insert_by_ref_called = true;
             });
        std::initializer_list<std::string> src
            {"something", "something", "something"};
        vec.assign(src.begin(), src.end());
        assert(after_insert_called);
        assert(after_insert_by_ref_called);
        check_equal(vec, {"something", "something", "something"});
    }
    
    //assign(initializer_list)
    {
        svector vec;
        bool after_insert_called{false};
        bool after_insert_by_ref_called{false};
        vec.after_insert
            ([&after_insert_called](svector& cont, svector::iterator it)
             {
                 assert(*it == "something");
                 after_insert_called = true;
             });
        vec.after_insert
            ([&after_insert_by_ref_called](svector::value_type& v)
             {
                 assert(v == "something");
                 after_insert_by_ref_called = true;
             });
        std::initializer_list<std::string> src
            {"something", "something", "something"};
        vec.assign(src);
        assert(after_insert_called);
        assert(after_insert_by_ref_called);
        check_equal(vec, {"something", "something", "something"});
    }
    
    //operator=(initializer_list)
    {
        svector vec;
        bool after_insert_called{false};
        bool after_insert_by_ref_called{false};
        vec.after_insert
            ([&after_insert_called](svector& cont, svector::iterator it)
             {
                 assert(*it == "something");
                 after_insert_called = true;
             });
        vec.after_insert
            ([&after_insert_by_ref_called](svector::value_type& v)
             {
                 assert(v == "something");
                 after_insert_by_ref_called = true;
             });
        vec = {"something", "something", "something"};
        assert(after_insert_called);
        assert(after_insert_by_ref_called);
        check_equal(vec, {"something", "something", "something"});
    }
    
    //swap()
    {
        svector vec{"something"};
        bool after_insert_called{false};
        bool after_insert_by_ref_called{false};
        bool before_erase_called{false};
        bool before_erase_by_ref_called{false};
        scoped_any_connection c = vec.before_erase
            ([&before_erase_called](svector& cont, svector::iterator it)
             {
                 assert(*it == "something");
                 before_erase_called = true;
             });
        scoped_any_connection c2 = vec.before_erase
             ([&before_erase_by_ref_called](svector::value_type& v)
             {
                 assert(v == "something");
                 before_erase_by_ref_called = true;
             });
        vec.after_insert
            ([&after_insert_called](svector& cont, svector::iterator first)
             {
                 assert(*first == "other");
                 after_insert_called = true;
             });
        svector other{"other", "other", "other"};
        vec.swap(other);
        assert(before_erase_called);
        assert(before_erase_by_ref_called);
        assert(after_insert_called);
        check_equal(vec, {"other", "other", "other"});
    }
    
    //begin()
    {
        svector vec;
        vec.emplace_back("abc");
        assert(*vec.begin() == "abc");
    }

    //cbegin()
    {
        svector vec;
        vec.emplace_back("abc");
        assert(*vec.cbegin() == "abc");
    }
    
    //rbegin()
    {
        svector vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        assert(*vec.rbegin() == "def");
    }
    
    //crbegin()
    {
        svector vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        assert(*vec.crbegin() == "def");
    }
    
    //end()
    {
        svector vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        assert(std::distance(vec.begin(), vec.end()) == 2);
    }

    //cend()
    {
        svector vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        assert(std::distance(vec.cbegin(), vec.cend()) == 2);
    }    

    //size()
    {
        svector vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        assert(vec.size() == 2);
    }
    
    //max_size()
    {
        svector vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        assert(vec.max_size() >= 2);
    }

    //resize(n)
    {
        svector vec{"abc", "def", "ghi"};
        bool before_erase_called{false};
        scoped_any_connection c = vec.before_erase
            ([&before_erase_called](svector& v, svector::iterator it)
             {
                 assert(*it == "ghi");
                 before_erase_called = true;
             });
        vec.resize(2);
        assert(before_erase_called);
    }
    
    //resize(n)
    {
        svector vec{"abc", "def", "ghi"};
        bool before_erase_called{false};
        scoped_any_connection c = vec.before_erase
            ([&before_erase_called](svector& v, svector::iterator it)
             {
                 assert(*it == "ghi");
                 before_erase_called = true;
             });
        vec.resize(2);
        assert(before_erase_called);
    }
    
    //resize(n, v)
    {
        svector vec{"abc", "def", "ghi"};
        bool before_erase_called{false};
        scoped_any_connection c = vec.before_erase
            ([&before_erase_called](svector& v, svector::iterator it)
             {
                 assert(*it == "ghi");
                 before_erase_called = true;
             });
        vec.resize(2, "something");
        assert(before_erase_called);
        bool after_insert_called{false};
        vec.after_insert
            ([&after_insert_called](svector& v,
                                    svector::iterator first)
             {
                 assert(*first == "something");
                 after_insert_called = true;
             });
        vec.resize(4, "something");
        assert(after_insert_called);
    }
    
    //empty()
    {
        svector vec;
        assert(vec.empty());
        vec.emplace_back("something");
        assert(!vec.empty());        
    }
        
    //reserve()
    {
        svector vec;
        vec.reserve(100);
    }
    
    //capacity()
    {
        svector vec;
        vec.emplace_back("abc");
        assert(vec.capacity() >= 1);
    }

    //shrink_to_fit()
    {
        svector vec;
        vec.emplace_back("abc");
        vec.shrink_to_fit();
    }
    
    //at fail
    {
        svector vec;
        bool ok{false};
        try
        {
            vec.at(0);
        }
        catch(const std::out_of_range&)
        { ok = true; }
        assert(ok);
    }

    //at success
    {
        bool ok{true};
        svector vec;
        vec.emplace(vec.cbegin(), "abc");
        try
        {            
            assert(vec.at(0) == "abc");
        }
        catch(const std::out_of_range&)
        { ok = false; }
        assert(ok);
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
        assert(ok);
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
            assert(cvalue == "abc");
        }
        catch(const std::out_of_range&)
        { ok = false; }
        assert(ok);            
    }

    //operator[] 
    {
        svector vec;
        vec.emplace(vec.cbegin(), "abc");
        assert(vec[0] == "abc");
    }

    //operator[] const
    {
        svector vec;
        vec.emplace(vec.cbegin(), "abc");
        const auto& cvec = vec;
        assert(cvec[0] == "abc");
    }
    
    //front()
    {
        svector vec;
        vec.emplace(vec.cbegin(), "abc");
        assert(vec.front() == "abc");
    }
    
    //front() const
    {
        svector vec;
        vec.emplace(vec.cbegin(), "abc");
        auto& cvec = vec;
        assert(cvec.front() == "abc");
    }
    
    //back()
    {
        svector vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        assert(vec.back() == "def");
    }
    
    //back() const
    {
        svector vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        const auto& cvec = vec;
        assert(cvec.back() == "def");
    }
        
    //clear() before_erase
    {
        svector vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        bool before_called{false};
        scoped_any_connection c = vec.before_erase([&before_called]
                         (svector& vec, svector::iterator it)
                         {
                             before_called = true;
                         });
        vec.clear();
        assert(before_called);
    }
                
    //erase() before_erase 
    {
        svector vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        bool before_called{false};
        scoped_any_connection c = vec.before_erase([&before_called]
                         (svector& vec, svector::iterator it)
                         {
                             before_called = true;
                             assert(*it == "abc");
                         });
        auto ret = vec.erase(vec.cbegin());
        assert(*ret == "def");
        assert(vec.size() == 1);
        assert(before_called);
    }
        
    //erase(first, last) before_erase 
    {
        svector vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        bool before_called{false};
        scoped_any_connection c = vec.before_erase([&before_called]
                          (svector& vec, svector::iterator first)
                         {
                             before_called = true;
                         });
        auto ret = vec.erase(vec.cbegin(), vec.cend());
        assert(ret == vec.end());
        assert(before_called);
        assert(vec.empty());
    }
    
    //emplace()
    {
        svector vec;
        bool called{false};
        vec.emplace_back("abc");
        vec.after_insert([&called](svector&,
                                   svector::iterator first)
                       {
                           assert(*first == "def");
                           called = true;
                       });
        vec.emplace(vec.cbegin(), "def");
        assert(called);
        assert(vec.front() == "def");
        assert(vec.size() == 2);
    }
    
    //insert(pos, const value_type&)
    {
        svector vec;
        std::string v = "abc";
        vec.insert(vec.cbegin(), v);
        bool called{false};
        vec.after_insert([&called](svector&,
                                   svector::iterator first)
                         {
                             assert(*first == "def");
                             called = true;
                         });
        std::string v2 = "def";
        vec.insert(std::next(vec.cbegin()), v2);
        assert(called);
        assert(vec.front() == "abc");
        assert(*std::next(vec.cbegin()) == "def");
        assert(vec.size() == 2);
    }

    //insert(pos, value_type&&)
    {
        svector vec;
        bool called{false};
        std::string v = "abc";
        vec.insert(vec.cbegin(), v);
        vec.after_insert([&called](svector&,
                                   svector::iterator first)
                         {
                             assert(*first == "def");
                             called = true;
                         });
        std::string v2 = "def";
        vec.insert(std::next(vec.cbegin()), v2);
        assert(called);
        assert(vec.front() == "abc");
        assert(*std::next(vec.cbegin()) == "def");
        assert(vec.size() == 2);
    }
    
    //insert(pos, count, const value_type&)
    {
        svector vec;
        bool called{false};
        vec.after_insert([&called](svector&,
                                   svector::iterator first)
                         {
                             assert(*first == "abc");
                             called = true;
                         });
        auto cit = vec.cbegin();
        vec.insert(cit, 2, "abc");
        assert(called);
        assert(*std::next(vec.cbegin()) == "abc");
        assert(vec.size() == 2);
    }
    
    //insert(pos, first, last)
    {
        svector vec;
        bool called{false};
        vec.after_insert([&called](svector& cont,
                                   svector::iterator first)
                         {
                             called = true;
                         });
        auto cit = vec.cbegin();
        std::initializer_list<std::string> src{"abc", "def"};
        vec.insert(cit, src.begin(), src.end());
        assert(called);
        assert(*std::next(vec.cbegin()) == "def");
        assert(vec.size() == 2);
    }

    //insert(ilist)
    {
        svector vec;
        bool called{false};
        vec.after_insert([&called](svector& cont,
                                   svector::iterator first)
                         {
                             called = true;
                         });
        auto cit = vec.cbegin();
        vec.insert(cit, {"abc", "def"});
        assert(called);
        assert(*std::next(vec.cbegin()) == "def");
        assert(vec.size() == 2);
    }
    
    //push_back()
    {
        svector vec;
        bool called{false};
        vec.emplace_back("abc");
        auto c =
            vec.after_insert([&called]
                           (svector&,
                            svector::iterator first)
                             {
                                 assert(*first == "def");
                                 called = true;
                             });
        vec.push_back(std::string{"def"});
        auto lvalue = std::string{"def"};
        vec.push_back(lvalue);
        assert(called);
        assert(vec.front() == "abc");
        assert(vec.back() == "def");
    }
    
    //emplace_back()
    {
        svector vec;
        bool called{false};
        vec.emplace_back("abc");
        auto c =
            vec.after_insert([&called]
                           (svector&,
                            svector::iterator first)
                             {
                                 assert(*first == "def");
                                 called = true;
                           });
        vec.emplace_back("def");
        assert(called);
        assert(vec.front() == "abc");
        assert(vec.back() == "def");
    }
    
    //pop_back() before_erase
    {
        svector vec;
        bool before_called{false};
        vec.emplace_back("abc");
        scoped_any_connection c = vec.before_erase([&before_called]
                         (svector::value_type v)
                         {
                             before_called = true;
                             assert(v == "abc");
                         });
        vec.pop_back();
        assert(before_called);
        assert(vec.empty());
    }

    //Signal emitted into a slot
    {
        svector vec{"current"};
        bool after_insert_called{false};
        bool before_erase_called{false};
        scoped_any_connection c = vec.before_erase([&before_erase_called]
                                                   (svector::value_type)
                         {
                             before_erase_called = true;
                         });
        vec.after_insert([&after_insert_called]
                         (svector& v, svector::iterator it)
                         {
                             after_insert_called = true;
                             v.erase(it);
                         });
        vec.emplace_back("changed");
        
        assert(after_insert_called);
        assert(before_erase_called);
    }
}
