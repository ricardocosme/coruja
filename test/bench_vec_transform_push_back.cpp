#include <benchmark/benchmark.h>
#include <boost/compressed_pair.hpp>
#include <coruja/container/vector.hpp>
#include <coruja/container/transform.hpp>
#include <string>

using namespace coruja;

struct B {
    std::string s;
};

struct A {
    std::string s;
    vector<B> b;
};

inline void body(benchmark::State& state, vector<A>& a)
{
    a.reserve(state.range(0));
    for(std::size_t i(0);i<state.range(0);++i)
        a.push_back(A{"abc"});
    a.clear();
}

void BM_AdHoc(benchmark::State& state) {
    vector<A> a;
    std::string s;
    a.for_each([&s](A& a)
               {
                   s = a.s;
                   a.b.for_each([&s](B& b){ s = b.s; });
               });
    for (auto _ : state)
        body(state, a);
}
BENCHMARK(BM_AdHoc)->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

template<typename T1, typename T2>
inline boost::compressed_pair<T1, T2>
row(T1&& t1, T2&& t2)
{ return{std::forward<T1>(t1), std::forward<T2>(t2)}; }

struct child_name {
    std::string& operator()(B& b) const
    { return b.s; }
};

void BM_Transform(benchmark::State& state) {
    vector<A> a;
    std::string s;
    auto rng = transform
        (a,
         [](A& a){ return row(a.s, transform(a.b, child_name{})); });
         // [](A& a){ return row(a.s, a.b); });

    using Row = ranges::range_reference_t<decltype(rng)>;
    rng.for_each([&s](Row&& row){
                     s = row.first();
                     row.second().for_each([&s](std::string& p){ s = p; });
                     // row.second().for_each([&s](B& b){ s = b.s; });
                 });
    for (auto _ : state)
        body(state, a);
}
BENCHMARK(BM_Transform)->Arg(1)->Arg(10)->Arg(100)->Arg(1000)->Arg(10000)->Arg(100000)->Arg(1000000);

BENCHMARK_MAIN();
