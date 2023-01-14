/**********************************************************************
 *  This file is part of arbxx.
 *
 *        Copyright (C)      2019 Vincent Delecroix
 *        Copyright (C) 2019-2022 Julian Rüth
 *
 *  arbxx is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  arbxx is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with arbxx. If not, see <https://www.gnu.org/licenses/>.
 *********************************************************************/

#include <benchmark/benchmark.h>

#include "../arbxx/arb.hpp"
#include "../test/arb.hpp"

namespace arbxx::test {

/// Benchmarks using the C++ interface provided by libarbxx.
struct ArbBenchmark_libarbxx : public benchmark::Fixture {};

/// Benchmarks using the original C interface provided by Arb.
struct ArbBenchmark_libarb : public benchmark::Fixture {};

/* Benchmark the Default Constructor */
BENCHMARK_DEFINE_F(ArbBenchmark_libarbxx, DefaultConstructor)(benchmark::State& state) {
  for (auto _ : state)
    benchmark::DoNotOptimize(Arf{});
}
BENCHMARK_REGISTER_F(ArbBenchmark_libarbxx, DefaultConstructor);

BENCHMARK_DEFINE_F(ArbBenchmark_libarb, DefaultConstructor)(benchmark::State& state) {
  for (auto _ : state) {
    arb_t x;
    arb_init(x);
    arb_clear(x);
  }
}
BENCHMARK_REGISTER_F(ArbBenchmark_libarb, DefaultConstructor);

/* Benchmark the Copy Constructor */
BENCHMARK_DEFINE_F(ArbBenchmark_libarbxx, CopyConstructor)(benchmark::State& state) {
  Arb a;

  for (auto _ : state)
    benchmark::DoNotOptimize(Arb{a});
}
BENCHMARK_REGISTER_F(ArbBenchmark_libarbxx, CopyConstructor);

BENCHMARK_DEFINE_F(ArbBenchmark_libarb, CopyConstructor)(benchmark::State& state) {
  arb_t x;
  arb_init(x);

  for (auto _ : state) {
    arb_t y;
    arb_init(y);
    arb_set(y, x);
    arb_clear(y);
  }

  arb_clear(x);
}
BENCHMARK_REGISTER_F(ArbBenchmark_libarb, CopyConstructor);

/* Benchmark the Move Constructor */
BENCHMARK_DEFINE_F(ArbBenchmark_libarbxx, MoveConstructor)(benchmark::State& state) {
  for (auto _ : state) {
    Arb a;
    benchmark::DoNotOptimize(Arb{std::move(a)});
  }
}
BENCHMARK_REGISTER_F(ArbBenchmark_libarbxx, MoveConstructor);

BENCHMARK_DEFINE_F(ArbBenchmark_libarb, MoveConstructor)(benchmark::State& state) {
  for (auto _ : state) {
    arb_t x, y;

    arb_init(x);
    arb_init(y);
    arb_swap(y, x);
    arb_clear(y);
    arb_clear(x);
  }
}
BENCHMARK_REGISTER_F(ArbBenchmark_libarb, MoveConstructor);

}  // namespace arbxx::test
