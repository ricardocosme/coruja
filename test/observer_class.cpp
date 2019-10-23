
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <boost/core/lightweight_test.hpp>

#include <coruja/container/vector.hpp>
#include <coruja/object/object.hpp>
#include <coruja/object/view/any_object.hpp>
#include <coruja/boost_optional.hpp>
#include <coruja/boost_variant.hpp>
#include <coruja/observer_class.hpp>

using namespace coruja;

struct empty{};

template<typename Self>
struct reaction_self
{
    void operator()(Self& self) const
    {
        BOOST_TEST(_self == &self);
        *self.called = true;
    }
    Self*& _self;
};

struct my_class_eb : observer_class<my_class_eb> {
    using base = observer_class<my_class_eb>;
    
    my_class_eb() = default;
    
    my_class_eb(bool& b)
        : base()
        , _self(new my_class_eb*{this})
        , called(&b)
    {
        observe(i, reaction_self<my_class_eb>{*_self});
        observe(ii, [](my_class_eb& self, int x)
                     {
                         *self.called = true;
                         BOOST_TEST(x == 10);
                     });
        observe(iii, [](my_class_eb& self, object<int>& x)
                     {
                         *self.called = true;
                         BOOST_TEST(x == 15);
                     });
        observe_for_each(v, [](my_class_eb& self, vector<int>&, vector<int>::iterator)
                          { *self.called = true; });
        observe_before_erase(v, [](my_class_eb& self, vector<int>&, vector<int>::iterator)
                              { *self.called = true; });
        observe(var, [](my_class_eb&){});
        observe(oi, [](my_class_eb&){});
    }

    my_class_eb(my_class_eb&& rhs)
        : base(std::move(rhs))
        , _self(std::move(rhs._self))
        , i(std::move(rhs.i))
        , ii(std::move(rhs.ii))
        , iii(std::move(rhs.iii))
        , v(std::move(rhs.v))
        , called(std::move(rhs.called))
        , var(std::move(rhs.var))
        , oi(std::move(rhs.oi))
    {
        *_self = this;
    }

    my_class_eb& operator=(my_class_eb&& rhs)
    {
        base::operator=(std::move(rhs));
        _self = std::move(rhs._self);
        i = std::move(rhs.i);
        ii = std::move(rhs.ii);
        iii = std::move(rhs.iii);
        v = std::move(rhs.v);
        called = std::move(rhs.called);
        var = std::move(rhs.var);
        oi = std::move(rhs.oi);
        *_self = this;
        return *this;
    }

    std::unique_ptr<my_class_eb*> _self;
    object<int> i, ii, iii;
    vector<int> v;
    bool* called{nullptr};
    variant<int, float> var;
    optional<int> oi;
};

template<typename Class>
void test_reactions(Class& o, bool& called)
{
    o.i = 5;
    BOOST_TEST(called);
    called = false;
    o.ii = 10;
    BOOST_TEST(called);
    called = false;
    o.iii = 15;
    BOOST_TEST(called);
    called = false;
    o.v.emplace_back(0);
    BOOST_TEST(called);
    called = false;
    o.v.erase(o.v.begin());
    BOOST_TEST(called);
    called = false;
}

struct observe_any_view : observer_class<observe_any_view> {
    observe_any_view(view::any_object<int>& o) {
        observe(o, []{});
        observe(o, [](observe_any_view&){});
        observe(o, [](observe_any_view&, int){});
        
        observe_obj_for_each(o, []{});
        observe_obj_for_each(o, [](observe_any_view&){});
        observe_obj_for_each(o, [](observe_any_view&, int){});
    }
};

int main() {

    static_assert(std::is_default_constructible<my_class_eb>::value,"");
    
    //reactions
    {
        bool called{false};
        my_class_eb o{called};
        test_reactions(o, called);
    }

    //move ctor
    {
        bool called{false};
        my_class_eb src{called};
        auto o = std::move(src);
        test_reactions(o, called);
    }

    //move assignment operator
    {
        bool called{false};
        my_class_eb src{called};
        my_class_eb o{called};
        o = std::move(src);
        test_reactions(o, called);
    }

    {
        object<int> o;
        view::any_object<int> v(o);
        observe_any_view c(v);
    }
}
