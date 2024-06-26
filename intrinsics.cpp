//TO COMPILE: g++ intrinsics.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -std=c++2a -O3 -fno-tree-vectorize -march=native -DNDEBUG -o intrinsics

#include <algorithm>
#include <benchmark/benchmark.h>
#include <numeric>
#include <x86intrin.h>

void BM_AddVectors(benchmark::State& state) {
  double data_a[4] = {(double) state.range(0), (double) state.range(1), (double) state.range(2), (double) state.range(3)};
  double data_b[4] = {(double) state.range(0), (double) state.range(1), (double) state.range(2), (double) state.range(3)};
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
BENCHMARK(BM_AddVectors)->Args({1, 2, 3, 4})->MinTime(0.5)->Repetitions(1000);

void BM_FindInVector(benchmark::State& state) {
  int target = state.range(0);
  int N = state.range(1);
  int vector[N];
  std::fill(vector, vector + N, 0);
  vector[state.range(2)] = target;
  int res = -1;

  for (auto _ : state) {
    __m256i x = _mm256_set1_epi32(target);

    for (int i = 0; i < N; i += 8) {
      __m256i y = _mm256_load_si256((__m256i*) &vector[i]);
      __m256i m = _mm256_cmpeq_epi32(x, y);
      int mask = _mm256_movemask_ps((__m256) m);
      if(mask != 0) {
        res = i + __builtin_ctz(mask);
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

  for (auto _ : state) {
    __m256i x = _mm256_set1_epi32(target);
    for (int i = 0; i < N; i += 32) {
      __m256i y1 = _mm256_load_si256((__m256i*) &vector[i]);
      __m256i m1 = _mm256_cmpeq_epi32(x, y1);
      __m256i y2 = _mm256_load_si256((__m256i*) &vector[i + 8]);
      __m256i m2 = _mm256_cmpeq_epi32(x, y2);
      __m256i y3 = _mm256_load_si256((__m256i*) &vector[i + 16]);
      __m256i m3 = _mm256_cmpeq_epi32(x, y3);
      __m256i y4 = _mm256_load_si256((__m256i*) &vector[i + 24]);
      __m256i m4 = _mm256_cmpeq_epi32(x, y4);
      __m256i m12 = _mm256_or_si256(m1, m2);
      __m256i m34 = _mm256_or_si256(m3, m4);
      __m256i m = _mm256_or_si256(m12, m34);
      if(!_mm256_testz_si256(m, m)) {
        int mask1 = _mm256_movemask_ps((__m256) m1);
        if (mask1) {
          res = i + __builtin_ctz(mask1);
          break;
        }
        int mask2 = _mm256_movemask_ps((__m256) m2);
        if (mask2) {
          res = i + 8 + __builtin_ctz(mask2);
          break;
        }
        int mask3 = _mm256_movemask_ps((__m256) m3);
        if (mask3) {
          res = i + 16 + __builtin_ctz(mask3);
          break;
        }
        int mask4 = _mm256_movemask_ps((__m256) m3);
        if (mask4) {
          res = i + 24 + __builtin_ctz(mask4);
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

  for (auto _ : state) {
    res = 0;
    __m256i s1 = _mm256_setzero_si256();
    __m256i s2 = _mm256_setzero_si256();
    
    for (int i = 0; i < N; i += 16) {
      s1 = _mm256_add_epi32(s1, _mm256_load_si256((__m256i*) &vector[i]));
      s2 = _mm256_add_epi32(s2, _mm256_load_si256((__m256i*) &vector[i + 8]));
    }

    __m256i s = _mm256_add_epi32(s1, s2);
    int t[8];

    _mm256_storeu_si256((__m256i*) t, s);
    
    for (int i = 0; i < 8; ++i) 
      res += t[i];

    benchmark::DoNotOptimize(res);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_SumVector)->Args({0, 4096})->MinTime(0.5)->Repetitions(1000);

void BM_ReverseVector(benchmark::State& state) {
  int N = state.range(1)-state.range(0);
  int vector[N];
  std::iota (vector, vector + N, state.range(0));
  const __m256i reversePermutation = _mm256_setr_epi32(7,6,5,4,3,2,1,0);

  for (auto _ : state) {
    for (int i = 0; i < N / 2; i += 8) {
      __m256i x = _mm256_loadu_si256((__m256i*) &vector[i]);
      __m256i y = _mm256_loadu_si256((__m256i*) &vector[N - i - 8]);
      _mm256_permutevar8x32_epi32(x, reversePermutation);
      _mm256_permutevar8x32_epi32(y, reversePermutation);
      _mm256_storeu_si256((__m256i*) &vector[N - i - 8], x);
      _mm256_storeu_si256((__m256i*) &vector[i], y);
    }

    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_ReverseVector)->Args({0, 4096})->MinTime(0.5)->Repetitions(1000);

BENCHMARK_MAIN();
