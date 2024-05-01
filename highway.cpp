//TO COMPILE: sudo g++ highway.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -std=c++2a -O3 -fno-tree-vectorize -march=native -DNDEBUG -I/usr/local/include/hwy -o highway

#include <numeric>
#define HWY_TARGETS N_AVX2
#include <algorithm>
#include <hwy/highway.h>
#include <benchmark/benchmark.h>

void BM_AddVectors(benchmark::State& state) {
  double data_a[4] = {(double) state.range(0), (double) state.range(1), (double) state.range(2), (double) state.range(3)};
  double data_b[4] = {(double) state.range(0), (double) state.range(1), (double) state.range(2), (double) state.range(3)};
  double result[4];

  for (auto _ : state) {
    const HWY_FULL(double) res;
    auto av = hwy::N_AVX2::Load(res, data_a);
    auto bv = hwy::N_AVX2::Load(res, data_b);
    auto rv = hwy::N_AVX2::Add(av, bv);
    hwy::N_AVX2::Store(rv, res, result);

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
    const HWY_FULL(int) d;
    auto x = hwy::N_AVX2::Set(d, target);

    for (int i = 0; i < N; i += 8) {
      auto y = hwy::N_AVX2::Load(d, &vector[i]);
      auto m = hwy::N_AVX2::Eq(x, y);
      if (!hwy::N_AVX2::AllFalse(d, m)) {
        res = i + hwy::N_AVX2::FindFirstTrue(d, m);
        break;
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
    const HWY_FULL(int) d;
    auto x = hwy::N_AVX2::Set(d, target);

    for (int i = 0; i < N; i += 32) {
      auto y1 = hwy::N_AVX2::Load(d, &vector[i]);
      auto m1 = hwy::N_AVX2::Eq(x, y1);
      auto y2 = hwy::N_AVX2::Load(d, &vector[i + 8]);
      auto m2 = hwy::N_AVX2::Eq(x, y2);
      auto y3 = hwy::N_AVX2::Load(d, &vector[i + 16]);
      auto m3 = hwy::N_AVX2::Eq(x, y3);
      auto y4 = hwy::N_AVX2::Load(d, &vector[i + 24]);
      auto m4 = hwy::N_AVX2::Eq(x, y4);
      auto m12 = hwy::N_AVX2::Or(m1, m2);
      auto m34 = hwy::N_AVX2::Or(m3, m4);
      auto m = hwy::N_AVX2::Or(m12, m34);
      if (!hwy::N_AVX2::AllFalse(d, m)) {
        if (!hwy::N_AVX2::AllFalse(d, m1)) {
          res = i + hwy::N_AVX2::FindFirstTrue(d, m1);
          break;
        }
        if (!hwy::N_AVX2::AllFalse(d, m2)) {
          res = i + hwy::N_AVX2::FindFirstTrue(d, m2) + 8;
          break;
        }
        if (!hwy::N_AVX2::AllFalse(d, m3)) {
          res = i + hwy::N_AVX2::FindFirstTrue(d, m3) + 16;
          break;
        }
        if (!hwy::N_AVX2::AllFalse(d, m4)) {
          res = i + hwy::N_AVX2::FindFirstTrue(d, m4) + 24;
          break;
        }
      }
    }

    benchmark::DoNotOptimize(res);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_FindInVectorFaster)->Args({456, 4096, 3254});

void BM_SumVector(benchmark::State& state) {
  int N = state.range(1)-state.range(0);
  int vector[N];
  std::iota (vector, vector + N, state.range(0));
  int res;

  for (auto _ : state) {
    res = 0;
    const HWY_FULL(int) d;
    auto s1 = hwy::N_AVX2::Set(d, 0);
    auto s2 = hwy::N_AVX2::Set(d, 0);
    
    for (int i = 0; i < N; i += 16) {
      auto simd_vector1 = hwy::N_AVX2::Load(d, &vector[i]);
      auto simd_vector2 = hwy::N_AVX2::Load(d, &vector[i + 8]);
      s1 = hwy::N_AVX2::Add(s1, simd_vector1);
      s2 = hwy::N_AVX2::Add(s2, simd_vector2);
    }

    auto s = hwy::N_AVX2::Add(s2, s1);
    int t[8];

    hwy::N_AVX2::Store(s, d, t);
    
    for (int i = 0; i < 8; ++i) 
      res += t[i];

    benchmark::DoNotOptimize(res);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_SumVector)->Args({0, 4096});

BENCHMARK_MAIN();
