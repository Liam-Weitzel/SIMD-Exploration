//TO COMPILE: g++ intrinsics.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -std=c++2a -O3 -fno-tree-vectorize -march=native -DNDEBUG -o intrinsics

#include <benchmark/benchmark.h>
#include <x86intrin.h>

void BM_AddVectors(benchmark::State& state) {
  double data_a[4] = {1.0, 2.0, 3.0, 4.0};
  double data_b[4] = {1.0, 2.0, 3.0, 4.0};
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
BENCHMARK(BM_AddVectors);
BENCHMARK_MAIN();
