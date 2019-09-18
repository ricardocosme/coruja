
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <coruja/object/object.hpp>
#include <coruja/object/object_view.hpp>
#include <coruja/container/container_view.hpp>
#include <coruja/container/vector.hpp>

using namespace coruja;

int main() {
    //We can't have a collision between the overload of the function
    //`view` that operates with ObservableObject and the version that
    //handles an ObservableErasableRange.
    {
        object<int> obj;
        auto obj_view = view(obj);
        auto another_obj_view = view(obj_view);
        
        vector<int> vec;
        auto container_view = view(vec);
        auto another_container_view = view(container_view);
    }
}
