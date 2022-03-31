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

#include <boost/lexical_cast.hpp>

#include "../arbxx/arb.hpp"
#include "external/catch2/single_include/catch2/catch.hpp"

#include "rand_generator.hpp"
#include "arb_generator.hpp"

namespace arbxx::test {

TEST_CASE("Arb", "[arb]") {
  flint_rand_t& state = GENERATE(rands());

  SECTION("Constructors") {
    SECTION("Default Constructor") {
      REQUIRE((Arb{} == 0) == true);
    }

    SECTION("Copy Constructor") {
      const auto& a = GENERATE_REF(take(128, arbs(state)));
      CAPTURE(a);

      Arb b{a};
      CAPTURE(b);

      REQUIRE(a.equal(b));
    }

    SECTION("Move Constructor") {
      auto& a = GENERATE_REF(take(128, arbs(state)));
      CAPTURE(a);

      const Arb b = a;
      CAPTURE(b);

      Arb c{std::move(a)};
      CAPTURE(c);

      REQUIRE(c.equal(a));
    }

    SECTION("Constructor from Integer") {
      const auto equal = [](const Arb& lhs, const auto& rhs) -> bool {
        auto interval = static_cast<std::pair<Arf, Arf>>(lhs);
        return interval.first == interval.second && interval.first == rhs;
      };

      SECTION("short") {
        REQUIRE(equal(Arb{static_cast<short>(0)}, 0));

        REQUIRE(equal(Arb{std::numeric_limits<short>::min()}, std::numeric_limits<short>::min()));
        REQUIRE(equal(Arb{std::numeric_limits<short>::max()}, std::numeric_limits<short>::max()));
      }

      SECTION("unsigned short") {
        REQUIRE(equal(Arb{static_cast<unsigned short>(0)}, 0));

        REQUIRE(equal(Arb{std::numeric_limits<unsigned short>::max()}, std::numeric_limits<unsigned short>::max()));
      }

      SECTION("int") {
        REQUIRE(equal(Arb{static_cast<int>(0)}, 0));

        REQUIRE(equal(Arb{std::numeric_limits<int>::min()}, std::numeric_limits<int>::min()));
        REQUIRE(equal(Arb{std::numeric_limits<int>::max()}, std::numeric_limits<int>::max()));
      }

      SECTION("unsigned int") {
        REQUIRE(equal(Arb{static_cast<unsigned int>(0)}, 0));

        REQUIRE(equal(Arb{std::numeric_limits<unsigned int>::max()}, std::numeric_limits<unsigned int>::max()));
      }

      SECTION("long") {
        REQUIRE(equal(Arb{static_cast<long>(0)}, 0));

        REQUIRE(equal(Arb{std::numeric_limits<long>::min()}, std::numeric_limits<long>::min()));
        REQUIRE(equal(Arb{std::numeric_limits<long>::max()}, std::numeric_limits<long>::max()));
      }

      SECTION("unsigned long") {
        REQUIRE(equal(Arb{static_cast<unsigned long>(0)}, 0));

        REQUIRE(equal(Arb{std::numeric_limits<unsigned long>::max()}, std::numeric_limits<unsigned long>::max()));
      }

      SECTION("long long") {
        REQUIRE(equal(Arb{static_cast<long long>(0)}, 0));

        REQUIRE(equal(Arb{std::numeric_limits<long long>::min()}, std::numeric_limits<long long>::min()));
        REQUIRE(equal(Arb{std::numeric_limits<long long>::max()}, std::numeric_limits<long long>::max()));
      }

      SECTION("unsigned long long") {
        REQUIRE(equal(Arb{static_cast<unsigned long long>(0)}, 0));

        REQUIRE(equal(Arb{std::numeric_limits<unsigned long long>::max()}, std::numeric_limits<unsigned long long>::max()));
      }

      // TODO (mpz & builtin types)
    }

    SECTION("Constructor from Rational") {
      // TODO
    }

    SECTION("Constructor from Arf") {
      // TODO (2)
    }
  }
}

}  // namespace arbxx::test
