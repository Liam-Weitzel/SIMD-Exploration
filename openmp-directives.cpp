//TO COMPILE: g++ openmp-directives.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -std=c++2a -O3 -fno-tree-vectorize -fopenmp -march=native -DNDEBUG -o openmp-directives

#include <benchmark/benchmark.h>

void BM_AddVectors(benchmark::State& state) {
  double data_a[4] = {1.0, 2.0, 3.0, 4.0};
  double data_b[4] = {1.0, 2.0, 3.0, 4.0};
  double result[4];

  for (auto _ : state) {
    #pragma omp simd
    for(int i = 0; i < 4; ++i) {
      result[i] = data_a[i] + data_b[i];
    }

    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_AddVectors);
BENCHMARK_MAIN();
