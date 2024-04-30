//TO COMPILE: g++ intrinsics.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -std=c++2a -O3 -fno-tree-vectorize -march=native -DNDEBUG -o intrinsics

#include <benchmark/benchmark.h>
#include <x86intrin.h>

void BM_AddVectors(benchmark::State& state) {
  double data_a[4] = {(double) state.range(0), (double) state.range(1), (double) state.range(2), (double) state.range(3)};
  double data_b[4] = {(double) state.range(0), (double) state.range(1), (double) state.range(2), (double) state.range(3)};
  double result[4];

  for (auto _ : state) {
    __m256d a = _mm256_loadu_pd(&data_a[0]);
    __m256d b = _mm256_loadu_pd(&data_b[0]);

    __m256d r = _mm256_add_pd(a, b);
    _mm256_storeu_pd(&result[0], r);

    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_AddVectors)->Args({1, 2, 3, 4});

void BM_FindInVector(benchmark::State& state) {
  int target = state.range(0);
  int N = state.range(1);
  int vector[N];
  vector[state.range(2)] = target;
  int res = -1;

  for (auto _ : state) {
    __m256i x = _mm256_set1_epi32(target);

    for (int i = 0; i < N; i += 8) {
      __m256i y = _mm256_load_si256((__m256i*) &vector[i]);
      __m256i m = _mm256_cmpeq_epi32(x, y);
      int mask = _mm256_movemask_ps((__m256) m);
      if(mask != 0) {
        res = i + __builtin_ctz(mask);
        break;
      }
    }
    
    benchmark::DoNotOptimize(res);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_FindInVector)->Args({456, 4096, 3254});

BENCHMARK_MAIN();
