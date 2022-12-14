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
#include <algorithm>

#include "../arbxx/arb.hpp"
#include "external/catch2/single_include/catch2/catch.hpp"

#include "rand_generator.hpp"
#include "arb_generator.hpp"
#include "arf_generator.hpp"

namespace arbxx::test {

TEST_CASE("Arb Constructors", "[arb][Arb]") {
  flint_rand_t& state = GENERATE(rands());

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

  const auto equal_point = [](const Arb& lhs, const auto& rhs) -> bool {
    auto interval = static_cast<std::pair<Arf, Arf>>(lhs);
    return interval.first == interval.second && interval.first == rhs;
  };

  SECTION("Constructor from Integer") {
    SECTION("short") {
      REQUIRE(equal_point(Arb{static_cast<short>(0)}, 0));

      REQUIRE(equal_point(Arb{std::numeric_limits<short>::min()}, std::numeric_limits<short>::min()));
      REQUIRE(equal_point(Arb{std::numeric_limits<short>::max()}, std::numeric_limits<short>::max()));
    }

    SECTION("unsigned short") {
      REQUIRE(equal_point(Arb{static_cast<unsigned short>(0)}, 0));

      REQUIRE(equal_point(Arb{std::numeric_limits<unsigned short>::max()}, std::numeric_limits<unsigned short>::max()));
    }

    SECTION("int") {
      REQUIRE(equal_point(Arb{static_cast<int>(0)}, 0));

      REQUIRE(equal_point(Arb{std::numeric_limits<int>::min()}, std::numeric_limits<int>::min()));
      REQUIRE(equal_point(Arb{std::numeric_limits<int>::max()}, std::numeric_limits<int>::max()));
    }

    SECTION("unsigned int") {
      REQUIRE(equal_point(Arb{static_cast<unsigned int>(0)}, 0));

      REQUIRE(equal_point(Arb{std::numeric_limits<unsigned int>::max()}, std::numeric_limits<unsigned int>::max()));
    }

    SECTION("long") {
      REQUIRE(equal_point(Arb{static_cast<long>(0)}, 0));

      REQUIRE(equal_point(Arb{std::numeric_limits<long>::min()}, std::numeric_limits<long>::min()));
      REQUIRE(equal_point(Arb{std::numeric_limits<long>::max()}, std::numeric_limits<long>::max()));
    }

    SECTION("unsigned long") {
      REQUIRE(equal_point(Arb{static_cast<unsigned long>(0)}, 0));

      REQUIRE(equal_point(Arb{std::numeric_limits<unsigned long>::max()}, std::numeric_limits<unsigned long>::max()));
    }

    SECTION("long long") {
      REQUIRE(equal_point(Arb{static_cast<long long>(0)}, 0));

      REQUIRE(equal_point(Arb{std::numeric_limits<long long>::min()}, std::numeric_limits<long long>::min()));
      REQUIRE(equal_point(Arb{std::numeric_limits<long long>::max()}, std::numeric_limits<long long>::max()));
    }

    SECTION("unsigned long long") {
      REQUIRE(equal_point(Arb{static_cast<unsigned long long>(0)}, 0));

      REQUIRE(equal_point(Arb{std::numeric_limits<unsigned long long>::max()}, std::numeric_limits<unsigned long long>::max()));
    }

    SECTION("mpz_class") {
      REQUIRE(equal_point(Arb{mpz_class{}}, 0));
    }
  }

  SECTION("Constructor from Rational") {
    REQUIRE(equal_point(Arb{mpq_class{1, 2}, 2} * 2, 1));
    REQUIRE(!equal_point(Arb{mpq_class{1, 3}, 2} * 3, 1));
  }

  SECTION("Constructor from Arf") {
    auto& a = GENERATE_REF(take(128, arfs(state)));

    REQUIRE(equal_point(Arb{a}, a));
  }

  SECTION("Constructor from String") {
    SECTION("Parse Errors are Reported as Exceptions") {
      REQUIRE_THROWS(Arb("error", 64));
    }

    SECTION("Invalid Precisions are Reported") {
      REQUIRE_THROWS(Arb("0", -1));
      REQUIRE_THROWS(Arb("0", 0));
      REQUIRE_NOTHROW(Arb("0", 1));
    }

    SECTION("Values Roundtrip") {
      auto& a = GENERATE_REF(take(1024, arbs(state)));

      const std::string s = static_cast<std::string>(a);

      Arb b{s, std::max(arb_bits(a), 1l)};

      CAPTURE(a, s, b, arb_dump_str(a));

      // This does not hold in general, see https://github.com/fredrik-johansson/arb/issues/412.
      // REQUIRE(a.equal(b));

      REQUIRE(b.contains(a));
    }
  }
}

TEST_CASE("Containment of Arb Elements", "[arb][contains]") {
  flint_rand_t& state = GENERATE(rands());

  auto& a = GENERATE_REF(take(128, arbs(state)));
  CAPTURE(a);

  REQUIRE(a.contains(a));
}

}  // namespace arbxx::test
