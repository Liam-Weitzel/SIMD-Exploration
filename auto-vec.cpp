//TO COMPILE: g++ auto-vec.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -std=c++2a -O3 -march=native -ftree-vectorize -DNDEBUG -o auto-vec

#include <algorithm>
#include <benchmark/benchmark.h>
#include <numeric>

void BM_AddVectors(benchmark::State& state) {
  double data_a[4] = {(double) state.range(0), (double) state.range(1), (double) state.range(2), (double) state.range(3)};
  double data_b[4] = {(double) state.range(0), (double) state.range(1), (double) state.range(2), (double) state.range(3)};
  double result[4];

  for (auto _ : state) {
    for(int i = 0; i < 4; ++i) {
      result[i] = data_a[i] + data_b[i];
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
    for (int i = 0; i < N; ++i) {
      if(vector[i] == target) res = i;
    }
    
    benchmark::DoNotOptimize(res);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_FindInVector)->Args({456, 4096, 3254})->MinTime(0.5)->Repetitions(1000);

void BM_SumVector(benchmark::State& state) {
  int N = state.range(1)-state.range(0);
  int vector[N];
  std::iota (vector, vector + N, state.range(0));
  int res;

  for (auto _ : state) {
    int res = 0;
    for( int i = 0; i < N; ++i ) {
      res += vector[i];
    }

    benchmark::DoNotOptimize(res);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_SumVector)->Args({0, 4096})->MinTime(0.5)->Repetitions(1000);

void BM_ReverseVector(benchmark::State& state) {
  int N = state.range(1)-state.range(0);
  int vector[N];
  std::iota (vector, vector + N, state.range(0));

  for (auto _ : state) {
    for (int i = 0; i < N / 2; ++i)
      std::swap(vector[i], vector[N - i - 1]);

    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_ReverseVector)->Args({0, 4096})->MinTime(0.5)->Repetitions(1000);

BENCHMARK_MAIN();
