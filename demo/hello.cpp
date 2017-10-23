
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <coruja/container/vector.hpp>

#include <iostream>
#include <string>

int main()
{    
    using namespace std;
    
    coruja::vector<string> v{"John Jones", "Robert Plant"};
    
    v.for_each([](auto& member){ cout << member << endl; });
    
    v.emplace_back("Jimmy Page");
    
    //Output:
    //John Jones
    //Robert Plant
    //Jimmy Page
}
