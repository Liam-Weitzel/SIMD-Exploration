//TO COMPILE: g++ std::experimental::simd.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -std=c++2a -O3 -fno-tree-vectorize -march=native -DNDEBUG -o std::experimental::simd

#include <algorithm>
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

void BM_FindInVector(benchmark::State& state) {
  int target = state.range(0);
  int N = state.range(1);
  int vector[N];
  std::fill(vector, vector + N, 0);
  vector[state.range(2)] = target;
  int res = -1;

  for (auto _ : state) {
    std::experimental::fixed_size_simd<int, 8> simd_target(target);

    for (int i = 0; i < N; i += 8) {
      std::experimental::fixed_size_simd<int, 8> simd_vector(&vector[i], std::experimental::vector_aligned);
      auto mask = simd_vector == simd_target;

      if (std::experimental::any_of(mask)) {
        for (int j = 0; j < 8; ++j) {
          if (mask[j]) {
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
BENCHMARK(BM_FindInVector)->Args({456, 4096, 3254});

void BM_FindInVectorFaster(benchmark::State& state) {
  int target = state.range(0);
  int N = state.range(1);
  int vector[N];
  std::fill(vector, vector + N, 0);
  vector[state.range(2)] = target;
  int res = -1;

  for (auto _ : state) {
    std::experimental::fixed_size_simd<int, 8> simd_target(target);

    for (int i = 0; i < N; i += 32) {
      std::experimental::fixed_size_simd<int, 8> simd_vector1(&vector[i], std::experimental::vector_aligned);
      std::experimental::fixed_size_simd<int, 8> simd_vector2(&vector[i + 8], std::experimental::vector_aligned);
      std::experimental::fixed_size_simd<int, 8> simd_vector3(&vector[i + 16], std::experimental::vector_aligned);
      std::experimental::fixed_size_simd<int, 8> simd_vector4(&vector[i + 24], std::experimental::vector_aligned);
      auto mask1 = simd_vector1 == simd_target;
      auto mask2 = simd_vector2 == simd_target;
      auto mask12 = mask1 || mask2;
      auto mask3 = simd_vector3 == simd_target;
      auto mask4 = simd_vector4 == simd_target;
      auto mask23 = mask3 || mask4;
      auto mask = mask12 || mask23;
      if (std::experimental::any_of(mask)) {
        if (std::experimental::any_of(mask1)) {
          for (int j = 0; j < 8; ++j) {
            if (mask1[j]) {
              res = i + j;
              break;
            }
          }
          break;
        }
        if (std::experimental::any_of(mask2)) {
          for (int j = 0; j < 8; ++j) {
            if (mask2[j]) {
              res = i + j + 8;
              break;
            }
          }
          break;
        }
        if (std::experimental::any_of(mask3)) {
          for (int j = 0; j < 8; ++j) {
            if (mask3[j]) {
              res = i + j + 16;
              break;
            }
          }
          break;
        }
        if (std::experimental::any_of(mask4)) {
          for (int j = 0; j < 8; ++j) {
            if (mask4[j]) {
              res = i + j + 24;
              break;
            }
          }
          break;
        }
      }
    }

    benchmark::DoNotOptimize(res);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_FindInVectorFaster)->Args({456, 4096, 3254});

BENCHMARK_MAIN();
