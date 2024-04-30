//TO COMPILE: g++ std::experimental::simd.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -std=c++2a -O3 -fno-tree-vectorize -march=native -DNDEBUG -o std::experimental::simd

#include <benchmark/benchmark.h>
#include <experimental/simd>

void BM_AddVectors(benchmark::State& state) {
  double data_a[4] = {(double) state.range(0), (double) state.range(1), (double) state.range(2), (double) state.range(3)};
  double data_b[4] = {(double) state.range(0), (double) state.range(1), (double) state.range(2), (double) state.range(3)};
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
BENCHMARK(BM_AddVectors)->Args({1, 2, 3, 4});
BENCHMARK_MAIN();
