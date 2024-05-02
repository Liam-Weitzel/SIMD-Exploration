#!/bin/bash
sudo cpupower frequency-set --governor performance
xset s 0 0 -dpms
export BENCHMARK_OUT_FORMAT=json
export BENCHMARK_OUT=inline-asm-data
./inline-asm
export BENCHMARK_OUT=intrinsics-data
./intrinsics
export BENCHMARK_OUT=highway-data
./highway
export BENCHMARK_OUT=eve-data
./eve
export BENCHMARK_OUT=std::experimental::simd-data
./std::experimental::simd
export BENCHMARK_OUT=xsimd-data
./xsimd
export BENCHMARK_OUT=openmp-directives-data
./openmp-directives
export BENCHMARK_OUT=auto-vec-data
./auto-vec
export BENCHMARK_OUT=no-vec-data
./no-vec
sudo cpupower frequency-set --governor powersave
xset s 60 60 +dpms
