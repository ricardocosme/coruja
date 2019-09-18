
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <coruja/object/view/lift.hpp>
#include <coruja/object/object.hpp>

#include <iostream>
#include <string>

using namespace coruja;

int main()
{    
    using namespace std;
    
    object<string> first_name, surname;
    
    auto fullname = first_name + surname;
    fullname.after_change([](auto&& s){ cout << s << endl; });
    
    first_name = "Jimmy";
    //output:
    //Jimmy
    
    surname = "Page";
    //output:
    //JimmyPage
}
