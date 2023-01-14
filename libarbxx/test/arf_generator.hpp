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

#ifndef LIBARBXX_TEST_ARF_GENERATOR_HPP
#define LIBARBXX_TEST_ARF_GENERATOR_HPP

#include <memory>
#include <boost/optional.hpp>

#include "../arbxx/arf.hpp"

#include "external/catch2/single_include/catch2/catch.hpp"

namespace arbxx::test {

/*
 * A generator of random Arf numbers.
 */
struct ArfGenerator : public Catch::Generators::IGenerator<Arf>
{
    flint_rand_t& state;
    ulong mantissa_bits, mag_bits;

    mutable boost::optional<Arf> current;

    ArfGenerator(flint_rand_t& state, ulong mantissa_bits, ulong mag_bits) : state(state), mantissa_bits(mantissa_bits), mag_bits(mag_bits) {
      current = Arf{};
    }

    bool next() override {
      if (current)
        current = {};
      return true;
    }

    Arf& get() const override {
      if (!current) {
        current = Arf{};
        arf_randtest_special(*current, state, mantissa_bits, mag_bits);
      }
      return *current;
    }
};

/*
 * Wrap ArfGenerator for use as GENERATE(arfs(...))
 */
inline Catch::Generators::GeneratorWrapper<Arf> arfs(flint_rand_t& state, ulong mantissa_bits = 64, ulong mag_bits = 16) {
  return Catch::Generators::GeneratorWrapper<Arf>(std::unique_ptr<Catch::Generators::IGenerator<Arf>>(new ArfGenerator(state, mantissa_bits, mag_bits)));
}

}

#endif
