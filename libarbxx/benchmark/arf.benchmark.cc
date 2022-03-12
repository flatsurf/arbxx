/**********************************************************************
 *  This file is part of arbxx.
 *
 *        Copyright (C) 2022 Julian Rüth
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

#include <boost/lexical_cast.hpp>

#include "../arbxx/arf.hpp"

namespace arbxx::test {

/// Benchmarks using the C++ interface provided by libarbxx.
struct ArfBenchmark_libarbxx : public benchmark::Fixture {};

/// Benchmarks using the original C interface provided by Arb.
struct ArfBenchmark_libarb : public benchmark::Fixture {};

BENCHMARK_DEFINE_F(ArfBenchmark_libarbxx, DefaultConstructor)(benchmark::State& state) {
  for (auto _ : state)
    benchmark::DoNotOptimize(Arf{});
}
BENCHMARK_REGISTER_F(ArfBenchmark_libarbxx, DefaultConstructor);

BENCHMARK_DEFINE_F(ArfBenchmark_libarb, DefaultConstructor)(benchmark::State& state) {
  for (auto _ : state) {
    arf_t x;
    arf_init(x);
    arf_clear(x);
  }
}
BENCHMARK_REGISTER_F(ArfBenchmark_libarb, DefaultConstructor);

BENCHMARK_DEFINE_F(ArfBenchmark_libarbxx, CopyConstructor)(benchmark::State& state) {
  Arf a;

  for (auto _ : state)
    benchmark::DoNotOptimize(Arf{a});
}
BENCHMARK_REGISTER_F(ArfBenchmark_libarbxx, CopyConstructor);

BENCHMARK_DEFINE_F(ArfBenchmark_libarb, CopyConstructor)(benchmark::State& state) {
  arf_t x;
  arf_init(x);

  for (auto _ : state) {
    arf_t y;
    arf_init(y);
    arf_set(y, x);
    arf_clear(y);
  }

  arf_clear(x);
}
BENCHMARK_REGISTER_F(ArfBenchmark_libarb, CopyConstructor);

BENCHMARK_DEFINE_F(ArfBenchmark_libarbxx, MoveConstructor)(benchmark::State& state) {
  for (auto _ : state) {
    Arf a;
    benchmark::DoNotOptimize(Arf{std::move(a)});
  }
}
BENCHMARK_REGISTER_F(ArfBenchmark_libarbxx, MoveConstructor);

BENCHMARK_DEFINE_F(ArfBenchmark_libarb, MoveConstructor)(benchmark::State& state) {
  for (auto _ : state) {
    arf_t x, y;

    arf_init(x);
    arf_init(y);
    arf_swap(y, x);
    arf_clear(y);
    arf_clear(x);
  }
}
BENCHMARK_REGISTER_F(ArfBenchmark_libarb, MoveConstructor);

BENCHMARK_DEFINE_F(ArfBenchmark_libarbxx, IntConstructor)(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(Arf{1337});
  }
}
BENCHMARK_REGISTER_F(ArfBenchmark_libarbxx, IntConstructor);

BENCHMARK_DEFINE_F(ArfBenchmark_libarb, IntConstructor)(benchmark::State& state) {
  for (auto _ : state) {
    arf_t x;

    arf_init_set_si(x, 1337);

    arf_clear(x);
  }
}
BENCHMARK_REGISTER_F(ArfBenchmark_libarb, IntConstructor);

BENCHMARK_DEFINE_F(ArfBenchmark_libarbxx, LongLongConstructor)(benchmark::State& state) {
  const long long value = state.range(0);

  for (auto _ : state) {
    benchmark::DoNotOptimize(Arf{value});
  }
}
BENCHMARK_REGISTER_F(ArfBenchmark_libarbxx, LongLongConstructor)->Arg(0)->Arg(std::numeric_limits<long long>::max());

BENCHMARK_DEFINE_F(ArfBenchmark_libarb, LongLongConstructor)(benchmark::State& state) {
  fmpz_t value;
  fmpz_init(value);
  fmpz_set_str(value, boost::lexical_cast<std::string>(state.range(0)).c_str(), 10);

  for (auto _ : state) {
    arf_t x;

    arf_init(x);

    arf_set_fmpz(x, value);

    arf_clear(x);
  }

  fmpz_clear(value);
}
BENCHMARK_REGISTER_F(ArfBenchmark_libarb, LongLongConstructor)->Arg(0)->Arg(std::numeric_limits<long long>::max());

}
