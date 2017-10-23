#include "check_equal.hpp"

#include <coruja/container/transform.hpp>
#include <coruja/container/vector.hpp>

#include <range/v3/range_concepts.hpp>
#include <string>

using namespace coruja;

using svector = vector<std::string>;

int main()
{    
    static_assert(std::is_same<container_view<svector>::observed_t,
                  svector::observed_t>::value, "");
    static_assert(std::is_same<container_view<svector>::for_each_connection_t,
                  svector::for_each_connection_t>::value, "");
    static_assert(std::is_same<container_view<svector>::before_erase_connection_t,
                  svector::before_erase_connection_t>::value, "");
    static_assert(ranges::Range<container_view<svector>>::value, "");

    {
        container_view<svector>();
    }

    //TODO
}
