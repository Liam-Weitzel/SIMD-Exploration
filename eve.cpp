//TO COMPILE: g++ eve.cpp -isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread -std=c++2a -O3 -fno-tree-vectorize -march=native -DNDEBUG -I/usr/local/include/eve -o eve

#include <benchmark/benchmark.h>
#include <eve/eve.hpp>

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
BENCHMARK(BM_AddVectors)->Args({1, 2, 3, 4});

void BM_FindInVector_Eve(benchmark::State& state) {
  int target = state.range(0);
  int N = state.range(1);
  int vector[N];
  vector[state.range(2)] = target;
  int res = -1;

  for (auto _ : state) {
    eve::wide<int, eve::fixed<8>> eve_target(target);
    int res = -1;

    for (int i = 0; i < N; i += 8) {
      eve::wide<int, eve::fixed<8>> eve_vector = eve::load(&vector[i]);
      auto matches = eve_target == eve_vector;
      auto index = eve::first_true(matches);

      if (index) {
        res = i + *index;
        break;
      }
    }

    benchmark::DoNotOptimize(res);
    benchmark::ClobberMemory();
  }
}
BENCHMARK(BM_FindInVector_Eve)->Args({456, 4096, 3254});

BENCHMARK_MAIN();
