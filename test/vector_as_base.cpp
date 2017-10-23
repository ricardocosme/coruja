#include <coruja/container/vector.hpp>

#include <string>

using namespace coruja;

struct my_vector;
using svector = vector<std::string,
                       std::allocator<std::string>,
                       std::vector,
                       my_vector>;

struct my_vector : svector
{
    using svector::svector;
};

int main()
{
    my_vector vec;
    vec.for_each([](my_vector&, my_vector::iterator){});
    vec.before_erase([](my_vector&, my_vector::iterator){});
}
