
#include "check_equal.hpp"

#include <coruja/container/list.hpp>

#include <string>

using namespace coruja;

using slist = list<std::string>;

int main()
{    
    static_assert(std::is_same<slist::model_t,
                  std::list<std::string>>::value, "");
    static_assert(std::is_same<slist::value_type,
                  std::list<std::string>::value_type>::value, "");
    static_assert(std::is_same<slist::allocator_type,
                  std::list<std::string>::allocator_type>::value, "");
    static_assert(std::is_same<slist::size_type,
                  std::list<std::string>::size_type>::value, "");
    static_assert(std::is_same<slist::difference_type,
                  std::list<std::string>::difference_type>::value, "");
    static_assert(std::is_same<slist::reference,
                  std::list<std::string>::reference>::value, "");
    static_assert(std::is_same<slist::const_reference,
                  std::list<std::string>::const_reference>::value, "");
    static_assert(std::is_same<slist::pointer,
                  std::list<std::string>::pointer>::value, "");
    static_assert(std::is_same<slist::const_pointer,
                  std::list<std::string>::const_pointer>::value, "");
    static_assert(std::is_same<slist::iterator,
                  std::list<std::string>::iterator>::value, "");
    static_assert(std::is_same<slist::const_iterator,
                  std::list<std::string>::const_iterator>::value, "");
    static_assert(std::is_same<slist::reverse_iterator,
                  std::list<std::string>::reverse_iterator>::value, "");
    static_assert(std::is_same<slist::const_reverse_iterator,
                  std::list<std::string>::const_reverse_iterator>::value, "");

    // static_assert(std::is_nothrow_move_constructible<slist>::value, "");
    // static_assert(std::is_nothrow_move_assignable<slist>::value, "");
        
    {
        slist lst;
    }

    {
        slist lst(3);
        assert(lst.size() == 3);
    }

    {
        slist lst(3, "abc");
        assert(lst.size() == 3);        
        check_equal(lst, {"abc", "abc", "abc"});
    }
    
    {
        std::list<std::string> src{"abc", "def", "ghi"};
        slist lst(src.begin(), src.end());
        assert(lst.size() == 3);        
        check_equal(lst, {"abc", "def", "ghi"});
    }
    
    {
        std::initializer_list<std::string> src{"abc", "def", "ghi"};
        slist lst(src);
        assert(lst.size() == 3);        
        check_equal(lst, {"abc", "def", "ghi"});
    }
    
    //list(list&&)
    {
        slist lst{"abc", "def", "ghi"};
        bool called{false};
        bool after_insert_called{false};
        lst.before_erase([&called](slist&, slist::iterator)
                         { called = true; });
        lst.after_insert([&after_insert_called](slist&, slist::iterator)
                         { after_insert_called = true; });
        auto lst2(std::move(lst));
        check_equal(lst2, {"abc", "def", "ghi"});
        lst2.emplace_back("jkl");
        lst2.erase(lst2.cbegin());
        assert(called);
        assert(after_insert_called);
    }
    
    //operator=(list&&)
    {
        slist lst{"abc", "def", "ghi"};
        bool called{false};
        bool after_insert_called{false};
        lst.before_erase([&called](slist&, slist::iterator)
                         { called = true; });
        lst.after_insert([&after_insert_called](slist&, slist::iterator)
                         { after_insert_called = true; });
        slist lst2;
        lst2 = std::move(lst);
        check_equal(lst2, {"abc", "def", "ghi"});
        lst2.emplace_back("jkl");
        lst2.erase(lst2.cbegin());
        assert(after_insert_called);
        assert(called);
    }
    
    //operator=(initializer_list)
    {
        slist lst;
        bool after_insert_called{false};
        lst.after_insert
            ([&after_insert_called](slist& cont,
                                    slist::iterator first)
             {
                 assert(*first == "something");
                 after_insert_called = true;
             });
        lst = {"something", "something", "something"};
        assert(after_insert_called);
        check_equal(lst, {"something", "something", "something"});
    }
    
    //begin()
    {
        slist lst;
        lst.emplace_back("abc");
        assert(*lst.begin() == "abc");
    }

    //cbegin()
    {
        slist lst;
        lst.emplace_back("abc");
        assert(*lst.cbegin() == "abc");
    }
    
    //rbegin()
    {
        slist lst;
        lst.emplace_back("abc");
        lst.emplace_back("def");
        assert(*lst.rbegin() == "def");
    }
    
    //crbegin()
    {
        slist lst;
        lst.emplace_back("abc");
        lst.emplace_back("def");
        assert(*lst.crbegin() == "def");
    }
    
    //end()
    {
        slist lst;
        lst.emplace_back("abc");
        lst.emplace_back("def");
        assert(std::distance(lst.begin(), lst.end()) == 2);
    }

    //cend()
    {
        slist lst;
        lst.emplace_back("abc");
        lst.emplace_back("def");
        assert(std::distance(lst.cbegin(), lst.cend()) == 2);
    }
    
    //rend()
    {
        slist lst;
        lst.emplace_back("abc");
        lst.emplace_back("def");
        assert(*std::next(lst.rend()) == "def");
    }
    
    //crend()
    {
        slist lst;
        lst.emplace_back("abc");
        lst.emplace_back("def");
        assert(*std::next(lst.crend()) == "def");
    }
    
    //size()
    {
        slist lst;
        lst.emplace_back("abc");
        lst.emplace_back("def");
        assert(lst.size() == 2);
    }
    
    //max_size()
    {
        slist lst;
        lst.emplace_back("abc");
        lst.emplace_back("def");
        assert(lst.max_size() >= 2);
    }
    
    //empty()
    {
        slist lst;
        assert(lst.empty());
        lst.emplace_back("something");
        assert(!lst.empty());        
    }
    
    //assign(n, val)
    {
        slist lst;
        bool before_erase_called{false};
        bool after_insert_called{false};
        lst.before_erase
            ([&before_erase_called](slist& cont, slist::iterator it)
             {
                 before_erase_called = true;
             });
        lst.after_insert
            ([&after_insert_called](slist& cont, slist::iterator it)
             {
                 assert(*it == "something");
                 after_insert_called = true;
             });
        lst.assign(3, "something");
        assert(!before_erase_called);
        assert(after_insert_called);
        check_equal(lst, {"something", "something", "something"});

        before_erase_called = false;
        after_insert_called = false;
        lst.assign(3, "something");
        assert(before_erase_called);
        assert(after_insert_called);
        check_equal(lst, {"something", "something", "something"});
    }
    
    //assign(first, last)
    {
        slist lst;
        bool after_insert_called{false};
        lst.after_insert
            ([&after_insert_called](slist& cont,
                                    slist::iterator first)
             {
                 assert(*first == "something");
                 after_insert_called = true;
             });
        std::initializer_list<std::string> src
            {"something", "something", "something"};
        lst.assign(src.begin(), src.end());
        assert(after_insert_called);
        check_equal(lst, {"something", "something", "something"});
    }
    
    //assign(initializer_list)
    {
        slist lst;
        bool after_insert_called{false};
        lst.after_insert
            ([&after_insert_called](slist& cont,
                                    slist::iterator first)
             {
                 assert(*first == "something");
                 after_insert_called = true;
             });
        std::initializer_list<std::string> src
            {"something", "something", "something"};
        lst.assign(src);
        assert(after_insert_called);
        check_equal(lst, {"something", "something", "something"});
    }    
    
    //swap()
    {
        slist lst{"something"};
        slist other{"other", "other", "other"};
        lst.swap(other);
        check_equal(lst, {"other", "other", "other"});
    }
    

    //resize(n)
    {
        slist lst{"abc", "def", "ghi"};
        bool before_erase_called{false};
        lst.before_erase
            ([&before_erase_called](slist& v, slist::iterator it)
             {
                 assert(*it == "ghi");
                 before_erase_called = true;
             });
        lst.resize(2);
        assert(before_erase_called);
    }
    
    //resize(n)
    {
        slist lst{"abc", "def", "ghi"};
        bool after_insert_called{false};
        lst.after_insert
            ([&after_insert_called](slist& v, slist::iterator it)
             {
                 assert(*it == "");
                 after_insert_called = true;
             });
        lst.resize(4);
        assert(after_insert_called);
    }
    
    //resize(n, v)
    {
        slist lst{"abc", "def", "ghi"};
        bool before_erase_called{false};
        lst.before_erase
            ([&before_erase_called](slist& v, slist::iterator it)
             {
                 assert(*it == "ghi");
                 before_erase_called = true;
             });
        lst.resize(2, "something");
        assert(before_erase_called);
        bool after_insert_called{false};
        lst.after_insert
            ([&after_insert_called](slist& v,
                                    slist::iterator first)
             {
                 assert(*first == "something");
                 after_insert_called = true;
             });
        lst.resize(4, "something");
        assert(after_insert_called);
    }
                    
    //front()
    {
        slist lst;
        lst.emplace(lst.cbegin(), "abc");
        assert(lst.front() == "abc");
    }
    
    //front() const
    {
        slist lst;
        lst.emplace(lst.cbegin(), "abc");
        auto& clst = lst;
        assert(clst.front() == "abc");
    }
    
    //back()
    {
        slist lst;
        lst.emplace_back("abc");
        lst.emplace_back("def");
        assert(lst.back() == "def");
    }
    
    //back() const
    {
        slist lst;
        lst.emplace_back("abc");
        lst.emplace_back("def");
        const auto& clst = lst;
        assert(clst.back() == "def");
    }
        
    //clear() before_erase
    {
        slist lst;
        lst.emplace_back("abc");
        lst.emplace_back("def");
        bool before_called{false};
        lst.before_erase([&before_called]
                         (slist& lst, slist::iterator it)
                         {
                             before_called = true;
                         });
        lst.clear();
        assert(before_called);
    }
                
    //erase() before_erase 
    {
        slist lst;
        lst.emplace_back("abc");
        lst.emplace_back("def");
        bool before_called{false};
        lst.before_erase([&before_called]
                         (slist& lst, slist::iterator it)
                         {
                             before_called = true;
                             assert(*it == "abc");
                         });
        auto ret = lst.erase(lst.cbegin());
        assert(*ret == "def");
        assert(lst.size() == 1);
        assert(before_called);
    }
        
    //erase(first, last) before_erase 
    {
        slist lst;
        lst.emplace_back("abc");
        lst.emplace_back("def");
        bool before_called{false};
        lst.before_erase([&before_called]
                          (slist& lst, slist::iterator first)
                         {
                             before_called = true;
                         });
        auto ret = lst.erase(lst.cbegin(), lst.cend());
        assert(ret == lst.end());
        assert(before_called);
        assert(lst.empty());
    }
    
    //emplace()
    {
        slist lst;
        bool called{false};
        lst.emplace_back("abc");
        lst.after_insert([&called](slist&,
                                   slist::iterator first)
                       {
                           assert(*first == "def");
                           called = true;
                       });
        lst.emplace(lst.cbegin(), "def");
        assert(called);
        assert(lst.front() == "def");
        assert(lst.size() == 2);
    }
    
    //insert(pos, const value_type&)
    {
        slist lst;
        std::string v = "abc";
        lst.insert(lst.cbegin(), v);
        bool called{false};
        lst.after_insert([&called](slist&,
                                   slist::iterator first)
                         {
                             assert(*first == "def");
                             called = true;
                         });
        std::string v2 = "def";
        lst.insert(std::next(lst.cbegin()), v2);
        assert(called);
        assert(lst.front() == "abc");
        assert(*std::next(lst.cbegin()) == "def");
        assert(lst.size() == 2);
    }

    //insert(pos, value_type&&)
    {
        slist lst;
        bool called{false};
        std::string v = "abc";
        lst.insert(lst.cbegin(), v);
        lst.after_insert([&called](slist&,
                                   slist::iterator first)
                         {
                             assert(*first == "def");
                             called = true;
                         });
        std::string v2 = "def";
        lst.insert(std::next(lst.cbegin()), v2);
        assert(called);
        assert(lst.front() == "abc");
        assert(*std::next(lst.cbegin()) == "def");
        assert(lst.size() == 2);
    }
    
    //insert(pos, count, const value_type&)
    {
        slist lst;
        bool called{false};
        lst.after_insert([&called](slist&,
                                   slist::iterator first)
                         {
                             assert(*first == "abc");
                             called = true;
                         });
        auto cit = lst.cbegin();
        lst.insert(cit, 2, "abc");
        assert(called);
        assert(*std::next(lst.cbegin()) == "abc");
        assert(lst.size() == 2);
    }
    
    //insert(pos, first, last)
    {
        slist lst;
        bool called{false};
        lst.after_insert([&called](slist& cont,
                                   slist::iterator first)
                         {
                             called = true;
                         });
        auto cit = lst.cbegin();
        std::initializer_list<std::string> src{"abc", "def"};
        lst.insert(cit, src.begin(), src.end());
        assert(called);
        assert(*std::next(lst.cbegin()) == "def");
        assert(lst.size() == 2);
    }

    //insert(ilist)
    {
        slist lst;
        bool called{false};
        lst.after_insert([&called](slist& cont,
                                   slist::iterator first)
                         {
                             called = true;
                         });
        auto cit = lst.cbegin();
        lst.insert(cit, {"abc", "def"});
        assert(called);
        assert(*std::next(lst.cbegin()) == "def");
        assert(lst.size() == 2);
    }
    
    //push_back()
    {
        slist lst;
        bool called{false};
        lst.push_front("abc");
        auto c =
            lst.after_insert([&called]
                           (slist&,
                            slist::iterator first)
                             {
                                 assert(*first == "def");
                                 called = true;
                             });
        lst.push_front(std::string{"def"});
        auto lvalue = std::string{"def"};
        lst.push_front(lvalue);
        assert(called);
        assert(lst.front() == "def");
        assert(lst.back() == "abc");
    }
    
    //push_back()
    {
        slist lst;
        bool called{false};
        lst.emplace_back("abc");
        auto c =
            lst.after_insert([&called]
                           (slist&,
                            slist::iterator first)
                             {
                                 assert(*first == "def");
                                 called = true;
                             });
        lst.push_back(std::string{"def"});
        auto lvalue = std::string{"def"};
        lst.push_back(lvalue);
        assert(called);
        assert(lst.front() == "abc");
        assert(lst.back() == "def");
    }
    
    //emplace_back()
    {
        slist lst;
        bool called{false};
        lst.emplace_back("abc");
        auto c =
            lst.after_insert([&called]
                           (slist&,
                            slist::iterator first)
                             {
                                 assert(*first == "def");
                                 called = true;
                           });
        lst.emplace_back("def");
        assert(called);
        assert(lst.front() == "abc");
        assert(lst.back() == "def");
    }
    
    //emplace_front()
    {
        slist lst;
        bool called{false};
        lst.emplace_front("abc");
        auto c =
            lst.after_insert([&called]
                           (slist&,
                            slist::iterator first)
                             {
                                 assert(*first == "def");
                                 called = true;
                           });
        lst.emplace_front("def");
        assert(called);
        assert(lst.front() == "def");
        assert(lst.back() == "abc");
    }
    
    //pop_back() before_erase
    {
        slist lst;
        bool before_called{false};
        lst.emplace_back("abc");
        lst.before_erase([&before_called]
                         (slist&, slist::iterator it)
                         {
                             before_called = true;
                             assert(*it == "abc");
                         });
        lst.pop_back();
        assert(before_called);
        assert(lst.empty());
    }

    //pop_back() before_erase
    {
        slist lst;
        bool before_called{false};
        lst.emplace_back("abc");
        lst.emplace_back("def");
        lst.before_erase([&before_called]
                         (slist&, slist::iterator it)
                         {
                             before_called = true;
                             assert(*it == "abc");
                         });
        lst.pop_front();
        assert(before_called);
        assert(lst.size() == 1);
    }
}
