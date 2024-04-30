//TO COMPILE: sudo g++ highway.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -std=c++2a -O3 -fno-tree-vectorize -march=native -DNDEBUG -I/usr/local/include/hwy -o highway

#include <hwy/highway.h>
#include <benchmark/benchmark.h>

void BM_AddVectors(benchmark::State& state) {
  double data_a[4] = {(double) state.range(0), (double) state.range(1), (double) state.range(2), (double) state.range(3)};
  double data_b[4] = {(double) state.range(0), (double) state.range(1), (double) state.range(2), (double) state.range(3)};
  double result[4];

  for (auto _ : state) {
    const HWY_FULL(double) res;
    auto av = hwy::N_AVX2::Load(res, data_a);
    auto bv = hwy::N_AVX2::Load(res, data_b);
    auto rv = hwy::N_AVX2::Add(av, bv);
    hwy::N_AVX2::Store(rv, res, result);

    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_AddVectors)->Args({1, 2, 3, 4});
BENCHMARK_MAIN();
