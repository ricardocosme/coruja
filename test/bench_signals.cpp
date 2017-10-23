#include <benchmark/benchmark.h>
#include <coruja/container/vector.hpp>
#include <coruja/support/boost_signals2.hpp>

using namespace coruja;

template<typename V>
inline void body(benchmark::State& state, V& v)
{
    for(std::size_t i(0);i<state.range(0);++i)
        v.push_back(i);
    state.PauseTiming();
    v.clear();
    state.ResumeTiming();
}


void wosignals(benchmark::State& state) {
    std::vector<std::size_t> v;
    v.reserve(state.range(0));
    for (auto _ : state) body(state, v);
}
BENCHMARK(wosignals)->Arg(5)->Arg(10000)->Arg(100000)->Arg(1000000);

void csignals(benchmark::State& state) {
    vector<std::size_t> v;
    v.reserve(state.range(0));
    for (auto _ : state) body(state, v);
}
BENCHMARK(csignals)->Arg(5)->Arg(10000)->Arg(100000)->Arg(1000000);

void bsignals(benchmark::State& state) {
    vector<std::size_t, std::allocator<std::size_t>, std::vector, void, boost_signals2> v;
    v.reserve(state.range(0));
    for (auto _ : state) body(state, v);
}
BENCHMARK(bsignals)->Arg(5)->Arg(10000)->Arg(100000)->Arg(1000000);

BENCHMARK_MAIN();
