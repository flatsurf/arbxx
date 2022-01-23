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
#include <cereal/archives/json.hpp>

#include "../arbxx/cereal.hpp"
#include "arb.hpp"
#include "arf.hpp"
#include "external/catch2/single_include/catch2/catch.hpp"

using cereal::JSONInputArchive;
using cereal::JSONOutputArchive;

// TODO: Simplify

namespace arbxx::test {

template <typename T>
std::string toString(const T& x) {
  return boost::lexical_cast<std::string>(x);
}

template <typename T>
T test_serialization(const T& x) {
  std::stringstream s;

  {
    JSONOutputArchive archive(s);
    archive(cereal::make_nvp("test", x));
  }

  T y;
  {
    JSONInputArchive archive(s);
    archive(cereal::make_nvp("test", y));
  }

  if constexpr (std::is_same_v<T, Arb>) {
    if (x.equal(y)) return y;
  } else {
    if (x == y) return y;
  }
  throw std::runtime_error("deserialization failed to reconstruct element, the original value " + toString(x) + " had serialized to " + s.str() + " which deserialized to " + toString(y));
}

TEST_CASE("Serialization of Arb", "[cereal][arb]") {
  ArbTester arbs;
  for (int i = 0; i < 1024; i++) {
    // Valgrind might report: Source and destination overlap in memcpy_chk.
    // This can be safely ignored: cereal's double conversion is calling
    // std::memmove which seems to intercepted incorrectly by valgrind.
    // (memmove can handle overlapping addresses.)
    test_serialization(arbs.random());
  }
}

TEST_CASE("Serialization of Arf", "[cereal][arf]") {
  ArfTester arfs;
  for (int i = 0; i < 1024; i++) {
    // Valgrind might report: Source and destination overlap in memcpy_chk.
    // This can be safely ignored: cereal's double conversion is calling
    // std::memmove which seems to intercepted incorrectly by valgrind.
    // (memmove can handle overlapping addresses.)
    test_serialization(arfs.random());
  }
}

}  // namespace arbxx::test
