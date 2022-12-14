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

#ifndef LIBARBXX_TEST_ARB_GENERATOR_HPP
#define LIBARBXX_TEST_ARB_GENERATOR_HPP

#include <memory>
#include <boost/optional.hpp>

#include "../arbxx/arb.hpp"

#include "external/catch2/single_include/catch2/catch.hpp"

namespace arbxx::test {

/*
 * A generator of random Arb numbers.
 */
struct ArbGenerator : public Catch::Generators::IGenerator<Arb>
{
    flint_rand_t& state;
    ulong mantissa_bits, mag_bits;

    mutable boost::optional<Arb> current;

    ArbGenerator(flint_rand_t& state, ulong mantissa_bits, ulong mag_bits) : state(state), mantissa_bits(mantissa_bits), mag_bits(mag_bits) {
      current = Arb{};
    }

    bool next() override {
      if (current)
        current = {};
      return true;
    }

    Arb& get() const override {
      if (!current) {
        current = Arb{};
        arb_randtest_special(*current, state, mantissa_bits, mag_bits);
      }
      return *current;
    }
};

/*
 * Wrap ArbGenerator for use as GENERATE(arbs(...))
 */
inline Catch::Generators::GeneratorWrapper<Arb> arbs(flint_rand_t& state, ulong mantissa_bits = 64, ulong mag_bits = 16) {
  return Catch::Generators::GeneratorWrapper<Arb>(std::unique_ptr<Catch::Generators::IGenerator<Arb>>(new ArbGenerator(state, mantissa_bits, mag_bits)));
}

}

#endif
