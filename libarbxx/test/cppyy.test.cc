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

#include "../arbxx/cppyy.hpp"
#include "external/catch2/single_include/catch2/catch.hpp"

namespace arbxx::test {

TEST_CASE("Test cppyy's C++ interface to Arb", "[arb][cppyy]") {
  // TODO
  /*
  Arb x(1);
  auto y = x + x;
  Arb z = arbxx::cppyy::eval(std::move(y), 10);
  REQUIRE(z.equal(Arb(2)));
  */
}

}  // namespace arbxx::test
