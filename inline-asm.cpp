//TO COMPILE: g++ inline-asm.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -std=c++2a -O3 -fno-tree-vectorize -march=native -DNDEBUG -o inline-asm
#include <benchmark/benchmark.h>

void BM_AddVectors(benchmark::State& state) {
  double data_a[4] = {1.0, 2.0, 3.0, 4.0};
  double data_b[4] = {1.0, 2.0, 3.0, 4.0};
  double result[4];

  for (auto _ : state) {
    asm volatile (
        "movdqu (%0), %%xmm0\n\t"         // Load data_a into xmm0
        "movdqu (%1), %%xmm1\n\t"         // Load data_b into xmm1
        "paddd %%xmm1, %%xmm0\n\t"        // Add xmm1 to xmm0
        "movupd %%xmm0, (%2)\n\t"         // Store result from xmm0 to result
        :                                  // No output
        : "r" (data_a), "r" (data_b), "r" (result)  // Input
        : "%xmm0", "%xmm1"                 // Clobbered registers
    );

    benchmark::DoNotOptimize(result);
    benchmark::ClobberMemory();
  }
}

BENCHMARK(BM_AddVectors);
BENCHMARK_MAIN();
