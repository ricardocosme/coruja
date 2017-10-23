
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <boost/core/lightweight_test.hpp>
#include <boost/serialization/string.hpp>

#include <boost/serialization/coruja_object.hpp>
#include <boost/serialization/coruja_vector.hpp>
#include <coruja/object/object.hpp>
#include "boost_serialization_tools.hpp"
#include "check_equal.hpp"

using namespace coruja;

struct X
{
    X() = delete;
    X(std::string ps) : s(std::move(ps)) {}

    std::string s ;
    
    template<class Archive>
    void serialize(Archive& ar, unsigned int)
    { ar & BOOST_SERIALIZATION_NVP(s); }
};

inline bool operator==(const X& lhs, const X& rhs)
{ return lhs.s == rhs.s; }

inline bool operator!=(const X& lhs, const X& rhs)
{ return !(lhs == rhs); }

template<class Archive>
inline void save_construct_data(
    Archive& ar, 
    const X* x,
    unsigned int)
{ ar << boost::serialization::make_nvp("s", x->s); }

template<class Archive>
inline void load_construct_data(
    Archive& ar, 
    X* x,
    unsigned int)
{
    std::string s;
    ar >> boost::serialization::make_nvp("s", s);
    ::new(x)X(s);
}

std::initializer_list<std::string> expected{"abc", "def", "ghi"};

struct my_vector : vector<std::string,
                          std::allocator<std::string>,
                          std::vector,
                          my_vector>
{
    using base = vector<std::string,
                        std::allocator<std::string>,
                        std::vector,
                        my_vector>;
    using base::base;

    template<typename Archive>
    void serialize(Archive& ar, unsigned int)
    { ar & static_cast<base&>(*this); }
};

int main()
{
    run([](std::string path)
        {
            vector<std::string> obj{expected};
            save_file(path, obj);
        },
        [](std::string path)
        {
            vector<std::string> obj;
            bool called{false};
            std::vector<std::string> vcbk;
            obj.for_each([&called, &vcbk](std::string s)
                         {
                             called = true;
                             vcbk.push_back(s);
                         });
            load_file(path, obj);
            BOOST_TEST(called);
            check_equal(obj, expected);
            check_equal(vcbk, expected);
        });
            
    static_assert(!boost::serialization::detail::is_default_constructible<
                  X>::value,"");
 
    run([](std::string path)
        {
            vector<X> obj(expected.begin(), expected.end());
            save_file(path, obj);
        },
        [](std::string path)
        {
            vector<X> obj;
            bool called{false};
            std::vector<std::string> vcbk;
            obj.for_each([&called, &vcbk](X x)
                         {
                             called = true;
                             vcbk.push_back(x.s);
                         });
            load_file(path, obj);
            BOOST_TEST(called);
            check_equal(obj, expected);
            check_equal(vcbk, expected);
        });
    
    run([](std::string path)
        {
            my_vector obj{expected};
            save_file(path, obj);
        },
        [](std::string path)
        {
            my_vector obj;
            bool called{false};
            std::vector<std::string> vcbk;
            obj.for_each([&called, &vcbk](std::string s)
                         {
                             called = true;
                             vcbk.push_back(s);
                         });
            load_file(path, obj);
            BOOST_TEST(called);
            check_equal(obj, expected);
            check_equal(vcbk, expected);
        });
}
