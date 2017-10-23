#include <benchmark/benchmark.h>
#include <coruja/object/lift.hpp>
#include <coruja/object/object.hpp>
#include <string>

using namespace coruja;

static void BM_Lift(benchmark::State& state) {
    object<std::string> o1, o2;
    o2 = "def";
    std::string r;
    auto concat = o1 + o2;
    concat.after_change([&](std::string s){ r = s; });
    std::size_t i;
    for (auto _ : state)
        o1 = "abc";
}
BENCHMARK(BM_Lift);

static void BM_AdHoc(benchmark::State& state) {
    object<std::string> o1, o2;
    o2 = "def";
    std::string r;
    auto l = [&](std::string){ r = o1.get() + o2.get(); };
    o1.after_change(l);
    o2.after_change(l);
    for (auto _ : state)
        o1 = "abc";
}
BENCHMARK(BM_AdHoc);

BENCHMARK_MAIN();
