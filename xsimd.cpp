//TO COMPILE: g++ xsimd.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -std=c++2a -O3 -fno-tree-vectorize -march=native -DNDEBUG -I/usr/local/include/xsimd -o xsimd

#define XSIMD_DEFAULT_ARCH xsimd::avx2
#include <numeric>
#include <algorithm>
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
BENCHMARK(BM_AddVectors)->Args({1, 2, 3, 4})->MinTime(0.5)->Repetitions(1000);

void BM_FindInVector(benchmark::State& state) {
  int target = state.range(0);
  int N = state.range(1);
  int vector[N];
  std::fill(vector, vector + N, 0);
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
        break;
      }
    }

    benchmark::DoNotOptimize(res);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_FindInVector)->Args({456, 4096, 3254})->MinTime(0.5)->Repetitions(1000);

void BM_FindInVectorFaster(benchmark::State& state) {
  int target = state.range(0);
  int N = state.range(1);
  int vector[N];
  std::fill(vector, vector + N, 0);
  vector[state.range(2)] = target;
  int res = -1;

  using batch_type = xsimd::batch<int, xsimd::avx2>;
  for (auto _ : state) {
    batch_type simd_target(target);

    for (int i = 0; i < N; i += 32) {
      batch_type simd_vector1 = xsimd::load_aligned(&vector[i]);
      batch_type simd_vector2 = xsimd::load_aligned(&vector[i + 8]);
      batch_type simd_vector3 = xsimd::load_aligned(&vector[i + 16]);
      batch_type simd_vector4 = xsimd::load_aligned(&vector[i + 24]);
      auto mask1 = simd_vector1 == simd_target;
      auto mask2 = simd_vector2 == simd_target;
      auto mask12 = mask1 || mask2;
      auto mask3 = simd_vector3 == simd_target;
      auto mask4 = simd_vector4 == simd_target;
      auto mask23 = mask3 || mask4;
      auto mask = mask12 || mask23;
      if (xsimd::any(mask)) {
        if (xsimd::any(mask1)) {
          for (int j = 0; j < 8; ++j) {
            if (mask1.get(j)) {
              res = i + j;
              break;
            }
          }
          break;
        }
        if (xsimd::any(mask2)) {
          for (int j = 0; j < 8; ++j) {
            if (mask2.get(j)) {
              res = i + j + 8;
              break;
            }
          }
          break;
        }
        if (xsimd::any(mask3)) {
          for (int j = 0; j < 8; ++j) {
            if (mask3.get(j)) {
              res = i + j + 16;
              break;
            }
          }
          break;
        }
        if (xsimd::any(mask4)) {
          for (int j = 0; j < 8; ++j) {
            if (mask4.get(j)) {
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
BENCHMARK(BM_FindInVectorFaster)->Args({456, 4096, 3254})->MinTime(0.5)->Repetitions(1000);

void BM_SumVector(benchmark::State& state) {
  int N = state.range(1)-state.range(0);
  int vector[N];
  std::iota (vector, vector + N, state.range(0));
  int res;

  using batch_type = xsimd::batch<int, xsimd::avx2>;
  for (auto _ : state) {
    res = 0;
    batch_type s1(0);
    batch_type s2(0);
    
    for (int i = 0; i < N; i += 16) {
      batch_type simd_vector1 = xsimd::load_aligned(&vector[i]);
      batch_type simd_vector2 = xsimd::load_aligned(&vector[i + 8]);
      s1 = s1 + simd_vector1;
      s2 = s2 + simd_vector2;
    }

    batch_type s = s1 + s2;
    int t[8];

    s.store_aligned(&t[0]);
    
    for (int i = 0; i < 8; ++i) 
      res += t[i];

    benchmark::DoNotOptimize(res);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_SumVector)->Args({0, 4096})->MinTime(0.5)->Repetitions(1000);

void BM_ReverseVector(benchmark::State& state) {
  int N = state.range(1) - state.range(0);
  int vector[N];
  std::iota (vector, vector + N, state.range(0));

  using batch_type = xsimd::batch<int, xsimd::avx2>;
  for (auto _ : state) {
    for (int i = 0; i < N / 2; i += 8) {
      batch_type simd_vector1 = xsimd::load_aligned(&vector[i]);
      batch_type simd_vector2 = xsimd::load_aligned(&vector[N - i - 8]);

      simd_vector1 = xsimd::rotate_left<8>(simd_vector1);
      simd_vector2 = xsimd::rotate_left<8>(simd_vector2);

      simd_vector2.store_aligned(&vector[i]);
      simd_vector1.store_aligned(&vector[N - i - 8]);
    }

    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_ReverseVector)->Args({0, 4096})->MinTime(0.5)->Repetitions(1000);

BENCHMARK_MAIN();
