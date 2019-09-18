
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <coruja/container/view/transform.hpp>
#include <coruja/container/vector.hpp>

#include <iostream>
#include <string>

using namespace coruja;

struct person_t { std::string first_name, surname; };

int main()
{    
    vector<person_t> persons;
    
    auto fullnames = view::transform
        (persons, [](auto& person){return person.first_name + person.surname;});
    
    using namespace std;
    
    persons.push_back({"John", "Jones"});
    persons.push_back({"Robert", "Plant"});
    
    fullnames.for_each
        ([](auto&& fullname){ cout << "hi " << fullname << endl; });
    
    fullnames.before_erase
        ([](auto&& fullname){ cout << "bye " << fullname << endl; });
    
    persons.push_back({"John", "Bonham"});
    persons.push_back({"Jimmy", "Page"});
}
