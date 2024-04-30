//TO COMPILE: g++ inline-asm.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -std=c++2a -O3 -fno-tree-vectorize -march=native -DNDEBUG -o inline-asm
#include <benchmark/benchmark.h>

void BM_AddVectors(benchmark::State& state) {
  double data_a[4] = {(double) state.range(0), (double) state.range(1), (double) state.range(2), (double) state.range(3)};
  double data_b[4] = {(double) state.range(0), (double) state.range(1), (double) state.range(2), (double) state.range(3)};
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
BENCHMARK(BM_AddVectors)->Args({1, 2, 3, 4});

void BM_FindInVector(benchmark::State& state) {
  int target = state.range(0);
  int N = state.range(1);
  int vector[N];
  vector[state.range(2)] = target;
  int res = -1;

  for (auto _ : state) {
    asm volatile (
      // Set target in all elements of a YMM register
      "movd %[target], %%xmm0\n\t"          // Move target into xmm0
      "vpermilps $0, %%xmm0, %%xmm0\n\t"    // Broadcast the integer across xmm0
      "vinsertf128 $1, %%xmm0, %%ymm0, %%ymm0\n\t" // Broadcast xmm0 to ymm0

      // Initialize loop variables
      "xor %%eax, %%eax\n\t"                // Clear eax for loop index

      ".p2align 4\n"                        // Align loop entry point to 16 bytes
      "1:\n\t"
      "vmovdqu (%[vec], %%rax, 4), %%ymm1\n\t" // Load 8 integers from vector
      "vpcmpeqd %%ymm0, %%ymm1, %%ymm2\n\t"    // Compare 8 integers with target
      "vmovmskps %%ymm2, %%edx\n\t"        // Move comparison mask to edx

      "test %%edx, %%edx\n\t"               // Test if any bits are set
      "jz 2f\n\t"                           // Jump to next iteration if none are set

      "tzcnt %%edx, %%edx\n\t"              // Count trailing zeros in mask
      "add %%eax, %%edx\n\t"                // Add base index to the bit position
      "mov %%edx, %[res]\n\t"               // Move result to output variable
      "jmp 3f\n\t"                          // Jump to end

      "2:\n\t"
      "add $8, %%eax\n\t"                   // Increment loop index
      "cmp %[N], %%eax\n\t"                 // Compare with N
      "jl 1b\n\t"                           // Jump back if not reached end

      "3:\n\t"

      : [res] "=r" (res)                    // Output
      : [target] "r" (target), [vec] "r" (vector), [N] "r" (N) // Inputs
      : "%eax", "%edx", "%xmm0", "%ymm0", "%ymm1", "%ymm2", "cc", "memory" // Clobbers
    );

    benchmark::DoNotOptimize(res);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_FindInVector)->Args({456, 4096, 3254});

BENCHMARK_MAIN();
