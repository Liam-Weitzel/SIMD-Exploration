//TO COMPILE: g++ std::experimental::simd.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -std=c++2a -O3 -fno-tree-vectorize -march=native -o std::experimental::simd

#include <benchmark/benchmark.h>
#include <experimental/simd>

void BM_AddVectors(benchmark::State& state) {
  double data_a[4] = {1.0, 2.0, 3.0, 4.0};
  double data_b[4] = {1.0, 2.0, 3.0, 4.0};
  double result[4];

  for (auto _ : state) {
    std::experimental::simd<double> a, b;
    a.copy_from(data_a, std::experimental::vector_aligned);
    b.copy_from(data_b, std::experimental::vector_aligned);
    auto simd_result = a + b;
    simd_result.copy_to(result, std::experimental::vector_aligned);

    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_AddVectors);
BENCHMARK_MAIN();
