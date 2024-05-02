//TO COMPILE: g++ eve.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -std=c++2a -O3 -fno-tree-vectorize -march=native -DNDEBUG -I/usr/local/include/eve -o eve

#include <benchmark/benchmark.h>
#include <algorithm>
#include <eve/eve.hpp>
#include <numeric>

void BM_AddVectors(benchmark::State& state) {
  double data_a[4] = {(double) state.range(0), (double) state.range(1), (double) state.range(2), (double) state.range(3)};
  double data_b[4] = {(double) state.range(0), (double) state.range(1), (double) state.range(2), (double) state.range(3)};
  double result[4];

  for (auto _ : state) {
    eve::wide<double, eve::fixed<4>> a = {data_a[0], data_a[1], data_a[2], data_a[3]};
    eve::wide<double, eve::fixed<4>> b = {data_b[0], data_b[1], data_b[2], data_b[3]};

    eve::wide<double, eve::fixed<4>> res = eve::add(a, b);

    for (int i = 0; i < res.size(); i++) {
      result[i] = res.get(i);
    }

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
    eve::wide<int, eve::fixed<8>> simd_target(target);

    for (int i = 0; i < N; i += 8) {
      eve::wide<int, eve::fixed<8>> simd_vector = eve::load(&vector[i]);
      auto matches = simd_target == simd_vector;
      if (eve::any(matches)) {
        auto index = eve::first_true(matches);
        res = i + *index;
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
    eve::wide<int, eve::fixed<8>> simd_target(target);

    for (int i = 0; i < N; i += 32) {
      eve::wide<int, eve::fixed<8>> simd_vector1 = eve::load(&vector[i]);
      eve::wide<int, eve::fixed<8>> simd_vector2 = eve::load(&vector[i + 8]);
      eve::wide<int, eve::fixed<8>> simd_vector3 = eve::load(&vector[i + 16]);
      eve::wide<int, eve::fixed<8>> simd_vector4 = eve::load(&vector[i + 24]);
      auto mask1 = simd_vector1 == simd_target;
      auto mask2 = simd_vector2 == simd_target;
      auto mask12 = mask1 || mask2;
      auto mask3 = simd_vector3 == simd_target;
      auto mask4 = simd_vector4 == simd_target;
      auto mask23 = mask3 || mask4;
      auto mask = mask12 || mask23;
      if (eve::any(mask)) {
        if(eve::any(mask1)) {
          auto index = eve::first_true(mask1);
          res = i + *index;
          break;
        }
        if(eve::any(mask2)) {
          auto index = eve::first_true(mask2);
          res = i + *index + 8;
          break;
        }
        if(eve::any(mask3)) {
          auto index = eve::first_true(mask3);
          res = i + *index + 16;
          break;
        }
        if(eve::any(mask4)) {
          auto index = eve::first_true(mask4);
          res = i + *index + 24;
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
    eve::wide<int, eve::fixed<8>> s1(0);
    eve::wide<int, eve::fixed<8>> s2(0);
    
    for (int i = 0; i < N; i += 16) {
      eve::wide<int, eve::fixed<8>> simd_vector1 = eve::load(&vector[i]);
      eve::wide<int, eve::fixed<8>> simd_vector2 = eve::load(&vector[i + 8]);
      s1 = s1 + simd_vector1;
      s2 = s2 + simd_vector2;
    }

    eve::wide<int, eve::fixed<8>> s = s1 + s2;
    int t[8];

    eve::store(s, t); 
    
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

  for (auto _ : state) {
    for (int i = 0; i < N / 2; i += 8) {
      eve::wide<int, eve::fixed<8>> simd_vector1 = eve::load(&vector[i]);
      eve::wide<int, eve::fixed<8>> simd_vector2 = eve::load(&vector[N - i - 8]);

      eve::reverse(simd_vector1);
      eve::reverse(simd_vector2);

      eve::store(simd_vector2, &vector[i]);
      eve::store(simd_vector1, &vector[N - i - 8]);
    }

    benchmark::ClobberMemory();
  }
}BENCHMARK(BM_ReverseVector)->Args({0, 4096})->MinTime(0.5)->Repetitions(1000);

BENCHMARK_MAIN();
