#pragma once

#include <boost/core/lightweight_test.hpp>
#include <coruja/container/vector.hpp>

using namespace coruja;

using ovector = vector<int>;

struct Cbk {
    void operator()(int) const {}
    explicit Cbk(bool& b) : copied(&b) {}
    Cbk(const Cbk& rhs)
        : copied(rhs.copied)
    { *copied = true; }
    Cbk& operator=(const Cbk& rhs)
    {
        copied = rhs.copied;
        *copied = true;
        return *this;
    }
    Cbk(Cbk&&) = default;
    Cbk& operator=(Cbk&&) = default;
    bool* copied;
};

template<typename F>
void run(ovector& v, F f)
{
    bool copied{false};
    f(v, Cbk{copied});
    BOOST_TEST(copied);
}

int main() {
    ovector v;
    run(v, [](ovector& v, Cbk&& cbk)
           { v.before_erase(cbk); });
    run(v, [](ovector& v, Cbk&& cbk)
           { v.for_each(cbk); });
}
