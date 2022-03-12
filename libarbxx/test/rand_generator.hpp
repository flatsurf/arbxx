/**********************************************************************
 *  This file is part of arbxx.
 *
 *        Copyright (C) 2020-2022 Julian Rüth
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

#ifndef LIBARBXX_TEST_RAND_GENERATOR_HPP
#define LIBARBXX_TEST_RAND_GENERATOR_HPP

#include <flint/flint.h>

#include "external/catch2/single_include/catch2/catch.hpp"

namespace arbxx::test {

/*
 * Generates a single flint_rand_t in a way that is compatible with Catch2 testing.

 */
struct RandGenerator : public Catch::Generators::IGenerator<flint_rand_t&> {
  flint_rand_t state;

  RandGenerator() {
    FLINT_GC_INIT();
    flint_randinit(state);
  }

  bool next() override {
    return false;
  }

  flint_rand_t& get() const override {
    return const_cast<flint_rand_t&>(state);
  }

  ~RandGenerator() override {
    flint_randclear(state);
    flint_cleanup_master();
  }
};

/*
 * Wrap RandGenerator for use in GENERATE(rands())
 */
[[maybe_unused]]
inline Catch::Generators::GeneratorWrapper<flint_rand_t&> rands() {
  return Catch::Generators::GeneratorWrapper<flint_rand_t&>(std::unique_ptr<Catch::Generators::IGenerator<flint_rand_t&>>(new RandGenerator()));
}

}

#endif
