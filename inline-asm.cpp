//TO COMPILE: g++ inline-asm.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -std=c++2a -O3 -fno-tree-vectorize -march=native -DNDEBUG -o inline-asm
#include <algorithm>
#include <benchmark/benchmark.h>
#include <numeric>

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
  std::fill(vector, vector + N, 0);
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

void BM_FindInVectorFaster(benchmark::State& state) {
  int target = state.range(0);
  int N = state.range(1);
  alignas(32) int vector[N];
  std::fill_n(vector, N, 0);
  vector[state.range(2)] = target;
  int res = -1;

  for (auto _ : state) {
    asm volatile (
        "vmovd %[target], %%xmm0\n\t"
        "vpbroadcastd %%xmm0, %%ymm0\n\t"
        "xor %%eax, %%eax\n\t" // Loop index set to zero
        "1:\n\t"
        "cmp %[N], %%eax\n\t"
        "jge 3f\n\t" // Jump to end if index exceeds N
                     // Load and compare sets of 8 integers
        "vmovdqu (%[vec], %%rax, 4), %%ymm1\n\t"
        "vpcmpeqd %%ymm0, %%ymm1, %%ymm1\n\t"
        "vmovdqu 32(%[vec], %%rax, 4), %%ymm2\n\t"
        "vpcmpeqd %%ymm0, %%ymm2, %%ymm2\n\t"
        "vmovdqu 64(%[vec], %%rax, 4), %%ymm3\n\t"
        "vpcmpeqd %%ymm0, %%ymm3, %%ymm3\n\t"
        "vmovdqu 96(%[vec], %%rax, 4), %%ymm4\n\t"
        "vpcmpeqd %%ymm0, %%ymm4, %%ymm4\n\t"
        // Combine results
        "vpor %%ymm2, %%ymm1, %%ymm5\n\t"
        "vpor %%ymm3, %%ymm4, %%ymm6\n\t"
        "vpor %%ymm5, %%ymm6, %%ymm7\n\t"
        "vmovmskps %%ymm7, %%edi\n\t"
        "test %%edi, %%edi\n\t"
        "jz 2f\n\t" // If zero, no matches, jump to next iteration
                    // Check each mask individually if combined mask shows a match
        "vmovmskps %%ymm1, %%edi\n\t"
        "test %%edi, %%edi\n\t"
        "jz 4f\n\t"
        "tzcnt %%edi, %%edi\n\t"
        "add %%eax, %%edi\n\t"
        "mov %%edi, %[res]\n\t"
        "jmp 3f\n\t"
        "4:\n\t"
        "vmovmskps %%ymm2, %%edi\n\t"
        "test %%edi, %%edi\n\t"
        "jz 5f\n\t"
        "tzcnt %%edi, %%edi\n\t"
        "add $8, %%edi\n\t"
        "add %%eax, %%edi\n\t"
        "mov %%edi, %[res]\n\t"
        "jmp 3f\n\t"
        "5:\n\t"
        "vmovmskps %%ymm3, %%edi\n\t"
        "test %%edi, %%edi\n\t"
        "jz 6f\n\t"
        "tzcnt %%edi, %%edi\n\t"
        "add $16, %%edi\n\t"
        "add %%eax, %%edi\n\t"
        "mov %%edi, %[res]\n\t"
        "jmp 3f\n\t"
        "6:\n\t"
        "vmovmskps %%ymm4, %%edi\n\t"
        "tzcnt %%edi, %%edi\n\t"
        "add $24, %%edi\n\t"
        "add %%eax, %%edi\n\t"
        "mov %%edi, %[res]\n\t"
        "2:\n\t"
        "add $32, %%eax\n\t"
        "jmp 1b\n\t" // Continue loop
        "3:\n\t"
        : [res] "+r" (res)  // Output
        : [target] "r" (target), [vec] "r" (vector), [N] "r" (N)  // Inputs
        : "eax", "edi", "xmm0", "ymm0", "ymm1", "ymm2", "ymm3", "ymm4", "ymm5", "ymm6", "ymm7", "cc", "memory"  // Clobbers
      );

    benchmark::DoNotOptimize(res);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_FindInVectorFaster)->Args({456, 4096, 3254});

void BM_SumVector(benchmark::State& state) {
  int N = state.range(1) - state.range(0);
  int vector[N];
  std::iota(vector, vector + N, state.range(0));
  int res;

  for (auto _ : state) {
    asm volatile (
      "vxorps %%ymm1, %%ymm1, %%ymm1\n\t" // Zero out ymm1
      "vxorps %%ymm2, %%ymm2, %%ymm2\n\t" // Zero out ymm2
      "mov %[N], %%ecx\n\t"               // Move N into ecx
      "lea %[vector], %%rdi\n\t"          // Load address of vector into rdi

      ".p2align 4\n\t"
      "1:\n\t"
      "vpaddd (%%rdi), %%ymm1, %%ymm1\n\t" // Add 8 integers from vector to ymm1
      "vpaddd 32(%%rdi), %%ymm2, %%ymm2\n\t" // Add next 8 integers to ymm2
      "add $64, %%rdi\n\t"               // Move to the next 16 integers
      "sub $16, %%ecx\n\t"               // Decrement loop counter by 16
      "jg 1b\n\t"                        // Jump back if still more than 16 elements left

      "vpaddd %%ymm2, %%ymm1, %%ymm1\n\t" // Add sums from ymm2 and ymm1
      "vextracti128 $1, %%ymm1, %%xmm2\n\t" // Extract the upper 128 bits
      "vpaddd %%xmm2, %%xmm1, %%xmm1\n\t" // Add upper 128 to lower 128
      "vpshufd $0x4e, %%xmm1, %%xmm2\n\t" // Shuffle to prepare for final addition
      "vpaddd %%xmm2, %%xmm1, %%xmm1\n\t" // Horizontal add
      "vpshufd $0xb1, %%xmm1, %%xmm2\n\t" // Another shuffle
      "vpaddd %%xmm2, %%xmm1, %%xmm1\n\t" // Add to get final sum in the lowest dword
      "vmovd %%xmm1, %[res]\n\t"          // Move result to scalar register

      : [res] "=r" (res) // Output
      : "[res]" (res), "m" (vector[0]), [vector] "m" (vector), [N] "r" (N) // Inputs
      : "%ymm1", "%ymm2", "%xmm1", "%xmm2", "%rdi", "%ecx", "memory", "cc" // Clobbers
    );

    benchmark::DoNotOptimize(res);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_SumVector)->Args({0, 4096});

BENCHMARK_MAIN();
