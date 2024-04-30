//TO COMPILE: g++ xsimd.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -std=c++2a -O3 -fno-tree-vectorize -march=native -DNDEBUG -I/usr/local/include/xsimd -o xsimd

#include <iostream>
#define XSIMD_DEFAULT_ARCH xsimd::avx2
#include "xsimd/xsimd.hpp"
#include <benchmark/benchmark.h>

void BM_AddVectors(benchmark::State& state) {
  double data_a[4] = {(double) state.range(0), (double) state.range(1), (double) state.range(2), (double) state.range(3)};
  double data_b[4] = {(double) state.range(0), (double) state.range(1), (double) state.range(2), (double) state.range(3)};
  double result[4];

  for (auto _ : state) {
    xsimd::batch<double, xsimd::avx2> a = xsimd::load_aligned(&data_a[0]);
    xsimd::batch<double, xsimd::avx2> b = xsimd::load_aligned(&data_a[0]);
    xsimd::batch<double, xsimd::avx2> res = a + b;

    res.store_aligned(&result[0]);

    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_AddVectors)->Args({1, 2, 3, 4});

void BM_FindInVector_xsimd(benchmark::State& state) {
  int target = state.range(0);
  int N = state.range(1);
  int vector[N];
  vector[state.range(2)] = target;
  int res = -1;

  using batch_type = xsimd::batch<int, xsimd::avx2>;
  for (auto _ : state) {
    batch_type simd_target(target);

    for (int i = 0; i < N; i += 8) {
      batch_type simd_vector = xsimd::load_aligned(&vector[i]);
      auto mask = simd_vector == simd_target;

      if (xsimd::any(mask)) {
        for (int j = 0; j < 8; ++j) {
          if (mask.get(j)) {
            res = i + j;
            break;
          }
        }
        if (res != -1) {
          break;
        }
      }
    }

    benchmark::DoNotOptimize(res);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_FindInVector_xsimd)->Args({456, 4096, 3254});

BENCHMARK_MAIN();
