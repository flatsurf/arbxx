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
#include <limits>

#include "../arbxx/arf.hpp"

#include "external/catch2/single_include/catch2/catch.hpp"

#include "rand_generator.hpp"
#include "arf_generator.hpp"

using boost::lexical_cast;
using std::string;

namespace arbxx::test {

TEST_CASE("Arf", "[arf]") {
  flint_rand_t& state = GENERATE(rands());

  SECTION("Constructors") {
    SECTION("Default Constructor") {
      REQUIRE(Arf{} == 0);
    }

    SECTION("Copy Constructor") {
      const auto& a = GENERATE_REF(take(128, arfs(state)));
      Arf b{a};

      REQUIRE(a == b);
    }

    SECTION("Move Constructor") {
      auto& a = GENERATE_REF(take(128, arfs(state)));

      const Arf b = a;

      Arf c{std::move(a)};

      REQUIRE(c == a);
    }

    SECTION("Constructor from Mantissa & Exponent") {
      SECTION("Base 2") {
        Arf zero{"", 2, 0};
        REQUIRE(zero == 0);

        Arf a{"101", 2, 1};
        REQUIRE(a == 0b1010);
      }

      SECTION("Base 8") {
        Arf zero{"", 8, 0};
        REQUIRE(zero == 0);

        Arf a{"1337", 8, 3};
        REQUIRE(a == 013370);
      }

      SECTION("Base 10") {
        Arf zero{"", 10, 0};
        REQUIRE(zero == 0);

        Arf a{"0815", 10, 0};
        REQUIRE(a == 815);

        REQUIRE(a == Arf{815, 0});
      }

      SECTION("Base 16") {
        Arf zero{"", 16, 0};
        REQUIRE(zero == 0);

        Arf a{"CAFE", 16, -1};
        REQUIRE(a == 0xCAFE/2);
      }
    }

    SECTION("Constructor from Elementary Integer Types") {
      SECTION("short") {
        REQUIRE(Arf{static_cast<short>(0)} == 0);

        REQUIRE(Arf{std::numeric_limits<short>::min()} == std::numeric_limits<short>::min());
        REQUIRE(Arf{std::numeric_limits<short>::max()} == std::numeric_limits<short>::max());
      }

      SECTION("unsigned short") {
        REQUIRE(Arf{static_cast<unsigned short>(0)} == 0);

        REQUIRE(Arf{std::numeric_limits<unsigned short>::max()} == std::numeric_limits<unsigned short>::max());
      }

      SECTION("int") {
        REQUIRE(Arf{static_cast<int>(0)} == 0);

        REQUIRE(Arf{std::numeric_limits<int>::min()} == std::numeric_limits<int>::min());
        REQUIRE(Arf{std::numeric_limits<int>::max()} == std::numeric_limits<int>::max());
      }

      SECTION("unsigned int") {
        REQUIRE(Arf{static_cast<unsigned int>(0)} == 0);

        REQUIRE(Arf{std::numeric_limits<unsigned int>::max()} == std::numeric_limits<unsigned int>::max());
      }

      SECTION("long") {
        REQUIRE(Arf{static_cast<long>(0)} == 0);

        REQUIRE(Arf{std::numeric_limits<long>::min()} == std::numeric_limits<long>::min());
        REQUIRE(Arf{std::numeric_limits<long>::max()} == std::numeric_limits<long>::max());
      }

      SECTION("unsigned long") {
        REQUIRE(Arf{static_cast<unsigned long>(0)} == 0);

        REQUIRE(Arf{std::numeric_limits<unsigned long>::max()} == std::numeric_limits<unsigned long>::max());
      }

      SECTION("long long") {
        REQUIRE(Arf{static_cast<unsigned long long>(0)} == 0);

        REQUIRE(Arf{std::numeric_limits<unsigned long long>::min()} == std::numeric_limits<unsigned long long>::min());
        REQUIRE(Arf{std::numeric_limits<unsigned long long>::max()} == std::numeric_limits<unsigned long long>::max());
      }

      SECTION("unsigned long long") {
        REQUIRE(Arf{static_cast<unsigned long long>(0)} == 0);

        REQUIRE(Arf{std::numeric_limits<unsigned long long>::max()} == std::numeric_limits<unsigned long long>::max());
      }
    }
  }

  SECTION("Constructor from Double") {
    REQUIRE(static_cast<double>(Arf(1.)) == 1.);
    REQUIRE(static_cast<double>(Arf(1./.0)) == 1./.0);
    REQUIRE(static_cast<double>(Arf(-1./.0)) == -1./.0);
    REQUIRE(static_cast<double>(Arf(.0/.0)) == .0/.0);
  }

  SECTION("Relational Operators") {
    Arf x(-1), y(1);

    REQUIRE(x < y);
    REQUIRE(y > x);
    REQUIRE(x <= y);
    REQUIRE(y >= x);
    REQUIRE(x == x);
    REQUIRE(x != y);
    REQUIRE(!(y < x));
    REQUIRE(!(x > y));
    REQUIRE(!(y <= x));
    REQUIRE(!(x >= y));
    REQUIRE(!(x == y));
    REQUIRE(!(x != x));
  }

  SECTION("Printing") {
    REQUIRE(lexical_cast<string>(Arf()) == "0");
    REQUIRE(lexical_cast<string>(Arf(1337)) == "1337");
    REQUIRE(lexical_cast<string>(Arf(13.37)) == "13.37=7526640877242941p-49");
  }

  SECTION("Floor & Ceil") {
    REQUIRE(Arf(.4).floor() == 0);
    REQUIRE(Arf(.4).ceil() == 1);
    REQUIRE(Arf(.6).floor() == 0);
    REQUIRE(Arf(.6).ceil() == 1);
  }
}

}  // namespace arbxx::test
