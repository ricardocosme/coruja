#include <algorithm>
#include <iomanip>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <coruja/container/vector.hpp>
#include <coruja/support/boost_signals2.hpp>
#include "timer.hpp"

using namespace coruja;

template<typename F>
float run(F&& f, std::size_t arg,
                std::string label,
                std::size_t reps = 1,
                std::size_t iter = 10000)
{
    using namespace boost;
    using namespace boost::accumulators;
    
    std::vector<float> rtimes;
    rtimes.reserve(reps);
    for(std::size_t r(0);r < reps;++r)
    {
        std::vector<std::size_t> times;
        times.reserve(iter);
        std::size_t sum{0};
        for(std::size_t i(0); i<iter; ++i)
            times.emplace_back(f(arg));
        accumulator_set<double, stats<tag::variance> > acc;
        acc = std::for_each(times.begin(), times.end(), acc);
        rtimes.emplace_back(mean(acc));
    }
    accumulator_set<double, stats<tag::variance> > acc;
    acc = std::for_each(rtimes.begin(), rtimes.end(), acc);
    auto _mean = mean(acc);
    std::cout << label << "/" << arg << "\t" 
              << "\tmean=" << std::fixed << std::setprecision(2) << _mean
              << "\tst=" << sqrt(variance(acc))
              << std::endl;
    return _mean;
}

template<typename V>
inline std::size_t test(V& v, std::size_t arg)
{
    v.reserve(arg);
    timer<std::chrono::nanoseconds> t;
    for(std::size_t i(0);i<arg;++i)
        v.push_back(i);
    return t.stop();
}

const std::size_t reps = 5;

float wsignals(std::size_t arg) {
    return run([](std::size_t arg) {
        std::vector<std::size_t> v;
        return test(v, arg);
        }, arg, "wsignals", reps);
}

float csignals(std::size_t arg) {
    return run([](std::size_t arg) {
        vector<std::size_t> v;
        return test(v, arg);
        }, arg, "csignals", reps);
}

float bsignals(std::size_t arg) {
    return run([](std::size_t arg) {
        vector<std::size_t, std::allocator<std::size_t>,
               std::vector, void, boost_signals2> v;
        return test(v, arg);
        }, arg, "bsignals", reps, 100);
}

int main() {
    std::initializer_list<std::size_t> il{5, 1000, 10000, 100000};
    for(auto n : il)
    {
        wsignals(n);
        auto v1 = csignals(n);
        auto v2 = bsignals(n);
        std::cout << "\t\tdiff=" << std::fixed << std::setprecision(2)
                  << (v2-v1)/v1 << std::endl;
    }
}
