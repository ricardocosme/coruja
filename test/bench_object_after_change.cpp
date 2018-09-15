#include <iostream>
#include <benchmark/benchmark.h>
#include <coruja/object/lift.hpp>
#include <coruja/object/object.hpp>
#include <string>
#include <vector>
using namespace coruja;

static void BM_AdHoc(benchmark::State& state) {
    object<std::string> o1, o2;
    o2 = "def";
    std::string r;
    auto l = [&r, &o1, &o2](std::string){ r = o1.get() + o2.get(); };
    for (auto _ : state)
    {
        std::vector<std::pair<
            object<std::string>::after_change_connection_t,
            object<std::string>::after_change_connection_t>> conns;
        conns.reserve(state.range(0));
        for(std::size_t i(0);i<state.range(0);++i)
        {
            auto c1 = o1.after_change(l);
            auto c2 = o2.after_change(l);
            conns.push_back({std::move(c1), std::move(c2)});
        }
        for(auto&& c : conns)
        {
            c.first.disconnect();
            c.second.disconnect();
        }
    }
}
BENCHMARK(BM_AdHoc)->Arg(1)->Arg(10)->Arg(100);

static void BM_Lift(benchmark::State& state) {
    object<std::string> o1, o2;
    o2 = "def";
    std::string r;
    auto concat = o1 + o2;
    for (auto _ : state)
    {
        std::vector<decltype(concat)::after_change_connection_t> conns;
        conns.reserve(state.range(0));
        for(std::size_t i(0);i<state.range(0);++i)
        {
            auto c = concat.after_change([&r](std::string s){ r = s; });
            conns.push_back(std::move(c));
        }
        for(auto&& c : conns) c.disconnect();
    }
}
BENCHMARK(BM_Lift)->Arg(1)->Arg(10)->Arg(100);

BENCHMARK_MAIN();
