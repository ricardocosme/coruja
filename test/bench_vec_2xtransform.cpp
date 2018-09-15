#include <benchmark/benchmark.h>
#include <coruja/container/vector.hpp>
#include <coruja/container/transform.hpp>
#include <string>

using namespace coruja;

inline void body(benchmark::State& state, vector<int>& a)
{
    a.reserve(state.range(0));
    for(std::size_t i(0);i<state.range(0);++i)
        a.push_back(i);
    a.clear();
}

void BM_AdHoc(benchmark::State& state) {
    std::string s;
    vector<int> a;
    a.for_each([&s](int i){ s = "_" + std::to_string(i); });
    for (auto _ : state)
        body(state, a);
}
BENCHMARK(BM_AdHoc)->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

void BM_Transform(benchmark::State& state) {
    std::string s;
    vector<int> a;
    auto rng = transform(transform(a, [](int i){return std::to_string(i);}),
                         [](std::string&& s){return "_" + std::move(s);});
    rng.for_each([&s](std::string&& v){ s = std::move(v); });
    for (auto _ : state)
        body(state, a);
}
BENCHMARK(BM_Transform)->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

BENCHMARK_MAIN();
