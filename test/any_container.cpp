
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <coruja/container/view/any_container.hpp>
#include <coruja/container/vector.hpp>
#include <coruja/container/map.hpp>
#include <coruja/container/view/transform.hpp>

#include <boost/core/lightweight_test.hpp>
#include <sstream>

#include "run_container.hpp"

using namespace coruja;

template<typename T>
using vec = std::vector<T>;
using ivec = vec<int>;
using svec = vec<std::string>;
using sivec = vec<std::pair<std::string, int>>;

using ivector = vector<int>;
using svector = vector<std::string>;
using simap = map<std::string,int>;
using iany_t = view::any_container<int>;
using sany_t = view::any_container<std::string>;
using siany_t = view::any_container<std::pair<std::string,int>>;

template<typename Cont>
struct Change;

template<typename T>
struct Change<vector<T>>
{
    template<typename V>
    V& operator()(V& c)
    {
        for(auto&& p : toRemove)
            cont->erase(cont->begin() + p);

        return c;
    }


    ivec toRemove;
    vector<T>* cont;
};

template<typename K, typename V>
struct Change<map<K,V>>
{
    template<typename View>
    View& operator()(View& c)
    {
        for(auto&& p : toRemove)
            cont->erase(p);

        return c;
    }


    std::vector<K> toRemove;
    map<K,V>* cont;
};

template<typename Cont, typename Rem>
Change<Cont> change(Cont& c, Rem toRemove)
{ return Change<Cont>{toRemove,&c}; }


struct keys
{
    template<typename pair>
    auto operator()(const pair& p) -> decltype(p.first)
    { return p.first; }
};

struct is_vowel
{
    template<typename T>
    bool operator()(const T& v)
    { return v == "a" || v == "e" || v == "i" || v == "o" || v == "u"; }
};

int main()
{
    //default ctor
    {
        iany_t iany;
        siany_t siany;
    }

    //from vector
    {
        ivector originator = { 1, 3, 5, 7, 9};
        iany_t any(originator);
        ivec expected_foreach = { 1, 3, 5, 7, 9};
        ivec keys_toRemove = {4};
        ivec erased_expected = {9};

        run_view(any, expected_foreach, change(originator, keys_toRemove), erased_expected );
    }

    //from map
    {
        simap originator = { {"a",1}, {"b", 3}, {"c",5}, {"d",7}, {"e", 9} };
        siany_t any(originator);
        sivec expected_foreach = { {"a",1}, {"b", 3}, {"c",5}, {"d",7}, {"e", 9} };
        svec keys_toRemove = { "c", "e" };
        sivec erased_expected = {{"c",5}, {"e", 9}};

        run_view(any, expected_foreach, change(originator, keys_toRemove), erased_expected);
    }

    //from transform
    {
        simap originator = { {"a",1}, {"b", 3}, {"c",5}, {"d",7}, {"e", 9} };
        sany_t any(view::transform(originator, keys{} ));
        svec expected_foreach = { "a", "b", "c", "d", "e" };
        svec keys_toRemove = { "c", "e" };
        svec erased_expected = { "c", "e" };

        run_view(any, expected_foreach, change(originator, keys_toRemove), erased_expected);
    }

    return 0;
}