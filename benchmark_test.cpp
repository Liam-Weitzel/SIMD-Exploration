//TO COMPILE: g++ mybenchmark.cc -std=c++11 -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -o mybenchmark

#include <benchmark/benchmark.h>
#include <experimental/simd>

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

void BM_SpinEmpty(benchmark::State& state) {
  for (auto _ : state) {
    for (int x = 0; x < state.range(0); ++x) {
      benchmark::DoNotOptimize(x);
    }
  }
}

BENCHMARK(BM_SpinEmpty)
  ->ComputeStatistics("max", [](const std::vector<double>& v) -> double {
    return *(std::max_element(std::begin(v), std::end(v)));
  })
  ->Arg(512);

void BM_AddVectorsStdSimd(benchmark::State& state) {
  for (auto _ : state) {
    int data_a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    int data_b[] = {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

    std::experimental::simd<int> a, b;
    a.copy_from(data_a, std::experimental::element_aligned);
    b.copy_from(data_b, std::experimental::element_aligned);

    auto result = a + b;
  }
}
BENCHMARK(BM_AddVectorsStdSimd);

BENCHMARK_MAIN();
