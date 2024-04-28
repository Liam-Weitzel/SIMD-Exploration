//TO COMPILE: g++ mybenchmark.cc -std=c++11 -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -o mybenchmark

#include <benchmark/benchmark.h>

static void BM_StringCreation(benchmark::State& state) {
  for (auto _ : state)
    std::string empty_string;
}
BENCHMARK(BM_StringCreation);

static void BM_StringCopy(benchmark::State& state) {
  std::string x = "hello";
  for (auto _ : state)
    std::string copy(x);
}
BENCHMARK(BM_StringCopy);

static void BM_IncrementBy(benchmark::State& state) {
  for (auto _ : state)
    int x = state.range(0) + state.range(1) + state.range(2) + state.range(3) + state.range(4);
}
BENCHMARK(BM_IncrementBy)->Args({1, 2, 3, 4, 5})->Args({5, 6, 7, 8, 9});

void BM_spin_empty(benchmark::State& state) {
  for (auto _ : state) {
    for (int x = 0; x < state.range(0); ++x) {
      benchmark::DoNotOptimize(x);
    }
  }
}

BENCHMARK(BM_spin_empty)
  ->ComputeStatistics("max", [](const std::vector<double>& v) -> double {
    return *(std::max_element(std::begin(v), std::end(v)));
  })
  ->Arg(512);

BENCHMARK_MAIN();
