#include "check_equal.hpp"

#include <coruja/container/view/container.hpp>
#include <coruja/container/vector.hpp>

#include <range/v3/range_concepts.hpp>
#include <string>

using namespace coruja;

using svector = vector<std::string>;

int main()
{    
    static_assert(std::is_same<view::container<svector>::observed_t,
                  svector::observed_t>::value, "");
    static_assert(std::is_same<view::container<svector>::for_each_connection_t,
                  svector::for_each_connection_t>::value, "");
    static_assert(std::is_same<view::container<svector>::before_erase_connection_t,
                  svector::before_erase_connection_t>::value, "");
    static_assert(ranges::Range<view::container<svector>>::value, "");

    {
        view::container<svector>();
    }

    //TODO
}
