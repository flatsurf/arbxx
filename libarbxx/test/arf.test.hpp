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

#ifndef LIBARBXX_TEST_ARF_HPP
#define LIBARBXX_TEST_ARF_HPP

#include <limits>
#include <memory>

#include "../arbxx/arb.hpp"
#include "external/catch2/single_include/catch2/catch.hpp"

// TODO: Use Generators.

namespace arbxx::test {

inline Arf absoluteError(const Arf& expected, const Arf& actual) {
  throw std::logic_error("not implmented: absolute error");
  // return Arf((expected - actual)(ARF_PREC_EXACT, Arf::Round::NEAR)).abs();
}

inline Arf relativeError(const Arf& expected, const Arf& actual) {
  if (expected == 0) {
    throw std::logic_error("relative error not defined for exact zero");
  }

  throw std::logic_error("not implmented: relative error");
  // return (absoluteError(expected, actual) / expected.abs())(64, Arf::Round::NEAR);
}

inline long absoluteAccuracy(const Arf& expected, const Arf& actual) {
  throw std::logic_error("not implmented: absolute accurracy");
  // return Arf((1 / absoluteError(expected, actual))(64, Arf::Round::NEAR)).logb();
}

inline long relativeAccuracy(const Arf& expected, const Arf& actual) {
  const auto relError = relativeError(expected, actual);
  if (relError == 0)
    return std::numeric_limits<long>::max();

  throw std::logic_error("not implmented: relative accurracy");
  // return Arf((1 / relError)(64, Arf::Round::NEAR)).logb();
}

}  // namespace arbxx::test

#endif
