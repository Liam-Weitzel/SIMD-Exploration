//TO COMPILE: g++ xsimd.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -std=c++2a -O3 -fno-tree-vectorize -march=native -DNDEBUG -I/usr/local/include/xsimd -o xsimd

#include "xsimd/xsimd.hpp"
#include <benchmark/benchmark.h>

void BM_AddVectors(benchmark::State& state) {
  double data_a[4] = {(double) state.range(0), (double) state.range(1), (double) state.range(2), (double) state.range(3)};
  double data_b[4] = {(double) state.range(0), (double) state.range(1), (double) state.range(2), (double) state.range(3)};
  double result[4];

  for (auto _ : state) {
    xsimd::batch<double, xsimd::avx2> a = {data_a[0], data_a[1], data_a[2], data_a[3]};
    xsimd::batch<double, xsimd::avx2> b = {data_b[0], data_b[1], data_b[2], data_b[3]};
    xsimd::batch<double, xsimd::avx2> res = a + b;

    res.store_aligned(&result[0]);

    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_AddVectors)->Args({1, 2, 3, 4});
BENCHMARK_MAIN();
