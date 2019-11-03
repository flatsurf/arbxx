/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2019 Vincent Delecroix
 *        Copyright (C) 2019 Julian Rüth
 *
 *  exact-real is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  exact-real is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with exact-real. If not, see <https://www.gnu.org/licenses/>.
 *********************************************************************/

#include <benchmark/benchmark.h>
#include <e-antic/renfxx.h>
#include <gtest/gtest.h>
#include <boost/lexical_cast.hpp>

#include <exact-real/element.hpp>
#include <exact-real/integer_ring.hpp>
#include <exact-real/module.hpp>
#include <exact-real/number_field.hpp>
#include <exact-real/rational_field.hpp>
#include <exact-real/real_number.hpp>

using boost::lexical_cast;
using eantic::renf_class;
using eantic::renf_elem_class;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::vector;

namespace exactreal {
TEST(Element, FromPrimitives) {
  auto m = Module<RationalField>::make({RealNumber::rational(1), RealNumber::random()});
  auto x = m->gen(1);

  auto one = Element(1);
  auto zero = Element(0);
  auto quot = Element(mpq_class(1, 2));

  // Check that arithmetic works with elements from different trivial rings
  EXPECT_EQ(one + zero, one);
  EXPECT_EQ(quot + zero, quot);
  EXPECT_EQ(quot + one, one + quot);
  
  EXPECT_EQ(x + zero, x);
  EXPECT_EQ(x + one - one, x);
  EXPECT_EQ(x + quot + quot, x + one);

  // Comparison between different parents in trivial cases
  EXPECT_EQ(quot - quot, zero);
  EXPECT_EQ(quot + quot, one);
  EXPECT_EQ(x - x, zero);
  EXPECT_EQ(x + one - x, one);
}

TEST(ElementZZ, Generators) {
  auto m = Module<IntegerRing>::make({RealNumber::rational(1), RealNumber::random()});

  auto one = m->gen(0);
  auto x = m->gen(1);

  EXPECT_EQ(one, one);
  EXPECT_EQ(x, x);
  EXPECT_EQ(x, m->gen(1));
  EXPECT_NE(one, x);
  EXPECT_GT(x, m->zero());
  EXPECT_LT(x, one);

  EXPECT_EQ(x[0], 0);
  EXPECT_EQ(x[1], 1);
}

TEST(ElementZZ, Additive) {
  auto m = Module<IntegerRing>::make({RealNumber::rational(1), RealNumber::random()});

  Element<IntegerRing> elements[]{m->gen(0), m->gen(1)};

  for (size_t i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    auto x = elements[i];
    EXPECT_GT(x + elements[0], x);
    EXPECT_GT(x + elements[0], elements[0]);
    EXPECT_EQ(x - elements[i], m->zero());
  }
}

TEST(ElementZZ, PromotionFromTrivial) {
  auto m = Module<IntegerRing>::make({RealNumber::rational(1), RealNumber::random()});

  auto gen = m->gen(1);
  auto zero = m->zero();
  auto trivial = Element<IntegerRing>();

  EXPECT_EQ(zero, trivial);
  EXPECT_NE(gen, trivial);

  EXPECT_EQ(gen + trivial, gen);
  EXPECT_EQ(zero + trivial, zero);
  EXPECT_EQ(0 * gen, trivial);
  EXPECT_GE(gen, trivial);
}

TEST(ElementZZ, PromotionFromSubmodule) {
  auto m = Module<IntegerRing>::make({RealNumber::rational(1)});
  auto n = Module<IntegerRing>::make({RealNumber::rational(1), RealNumber::random()});

  EXPECT_EQ(m->gen(0), n->gen(0));
  EXPECT_NE(m->gen(0), n->gen(1));
  EXPECT_EQ(&*(m->gen(0) + n->gen(0)).module(), &*n);
}

TEST(ElementZZ, Scalars) {
  auto m = Module<IntegerRing>::make({RealNumber::rational(1), RealNumber::random()});

  Element<IntegerRing> elements[]{m->gen(0), m->gen(1)};

  for (size_t i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    auto x = elements[i];
    EXPECT_EQ(x + x, 2 * x);
    EXPECT_EQ(x - x, 0 * x);
    EXPECT_EQ(-1 * x, -x);
    EXPECT_EQ(1 * x, x);
    EXPECT_EQ(0 * x, m->zero());
    EXPECT_EQ(Element<IntegerRing>() * x, Element<IntegerRing>());
  }

  for (size_t i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    auto x = elements[i];
    EXPECT_EQ(x + x, mpz_class(2) * x);
    EXPECT_GT(mpz_class(2) * x, x);
    EXPECT_EQ(x - x, mpz_class(0) * x);
    EXPECT_EQ(mpz_class(-1) * x, -x);
    EXPECT_LT(mpz_class(-1) * x, x);
    EXPECT_LT(mpz_class(-1) * x, m->zero());
    auto one = RealNumber::rational(1);
    EXPECT_LT(mpz_class(-1) * x, *one);
    if (i == 0)
      EXPECT_EQ(x, *one);
    else
      EXPECT_NE(x, *one);
    EXPECT_EQ(mpz_class(1) * x, x);
    EXPECT_EQ(mpz_class(0) * x, m->zero());
  }
}

TEST(ElementZZ, Printing) {
  auto m = Module<IntegerRing>::make({RealNumber::rational(1), RealNumber::random()});

  EXPECT_EQ(lexical_cast<string>(m->gen(0)), "1");
  EXPECT_EQ(lexical_cast<string>(m->gen(1)), "ℝ(0.982253…)");
  EXPECT_EQ(lexical_cast<string>(m->gen(0) + m->gen(1)), "ℝ(0.982253…) + 1");
}

TEST(ElementZZ, Multiplication) {
  auto m = Module<IntegerRing>::make({RealNumber::rational(1), RealNumber::random()});

  auto one = m->gen(0);
  auto rnd = m->gen(1);

  EXPECT_EQ(one * one, one);
  EXPECT_EQ(one * rnd, rnd);
  EXPECT_NE(rnd * rnd, rnd);
  EXPECT_EQ(rnd * rnd, rnd * rnd);
  EXPECT_EQ(rnd - rnd * one, one - one);
}

TEST(ElementQQ, Scalars) {
  auto m = Module<RationalField>::make({RealNumber::rational(1), RealNumber::random()});

  Element<RationalField> elements[]{m->gen(0), m->gen(1)};

  for (size_t i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    auto x = elements[i];
    EXPECT_EQ(x + x, 2 * x);
    EXPECT_EQ(x - x, 0 * x);
    EXPECT_EQ(-1 * x, -x);
    EXPECT_EQ(1 * x, x);
    EXPECT_EQ(0 * x, m->zero());
    EXPECT_EQ(Element<RationalField>() * x, Element<RationalField>());
  }

  for (size_t i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    auto x = elements[i];
    EXPECT_EQ(x + x, mpz_class(2) * x);
    EXPECT_GT(mpz_class(2) * x, x);
    EXPECT_EQ(x - x, mpz_class(0) * x);
    EXPECT_EQ(mpz_class(-1) * x, -x);
    EXPECT_LT(mpz_class(-1) * x, x);
    EXPECT_LT(mpz_class(-1) * x, m->zero());
    auto one = RealNumber::rational(1);
    EXPECT_LT(mpz_class(-1) * x, *one);
    if (i == 0)
      EXPECT_EQ(x, *one);
    else
      EXPECT_NE(x, *one);
    EXPECT_EQ(mpz_class(1) * x, x);
    EXPECT_EQ(mpz_class(0) * x, m->zero());
  }

  for (size_t i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    auto x = elements[i];
    EXPECT_EQ(x + x, mpq_class(2) * x);
    EXPECT_EQ(x - x, mpq_class(0) * x);
    EXPECT_EQ(mpq_class(-1) * x, -x);
    EXPECT_EQ(mpq_class(1) * x, x);
    EXPECT_EQ(mpq_class(0) * x, m->zero());
  }
}

TEST(ElementNF, Scalars) {
  auto K = renf_class::make("a^2 - 2", "a", "1.41 +/- 0.1", 64);
  auto m = Module<NumberField>::make({RealNumber::rational(1), RealNumber::random()}, K);

  Element<NumberField> elements[]{m->gen(0), m->gen(1)};

  for (size_t i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    auto x = elements[i];
    EXPECT_EQ(x + x, 2 * x);
    EXPECT_EQ(x - x, 0 * x);
    EXPECT_EQ(-1 * x, -x);
    EXPECT_EQ(1 * x, x);
    EXPECT_EQ(0 * x, m->zero());
    EXPECT_EQ(Element<NumberField>() * x, Element<NumberField>());
  }

  for (size_t i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    auto x = elements[i];
    EXPECT_EQ(x + x, mpz_class(2) * x);
    EXPECT_GT(mpz_class(2) * x, x);
    EXPECT_EQ(x - x, mpz_class(0) * x);
    EXPECT_EQ(mpz_class(-1) * x, -x);
    EXPECT_LT(mpz_class(-1) * x, x);
    EXPECT_LT(mpz_class(-1) * x, m->zero());
    auto one = RealNumber::rational(1);
    EXPECT_LT(mpz_class(-1) * x, *one);
    if (i == 0)
      EXPECT_EQ(x, *one);
    else
      EXPECT_NE(x, *one);
    EXPECT_EQ(mpz_class(1) * x, x);
    EXPECT_EQ(mpz_class(0) * x, m->zero());
  }

  for (size_t i = 0; i < sizeof(elements) / sizeof(elements[0]); i++) {
    auto x = elements[i];
    EXPECT_EQ(x + x, renf_elem_class(K, 2) * x);
    EXPECT_EQ(x - x, renf_elem_class(K, 0) * x);
    EXPECT_EQ(renf_elem_class(K, -1) * x, -x);
    EXPECT_EQ(renf_elem_class(K, 1) * x, x);
    EXPECT_EQ(renf_elem_class(K, 0) * x, m->zero());
    EXPECT_EQ(-(renf_elem_class(K, "a") * x), renf_elem_class(K, "-a") * x);
    EXPECT_GT(renf_elem_class(K, "a") * x, x);
    EXPECT_GT(2 * x, renf_elem_class(K, "a") * x);
    EXPECT_EQ(mpq_class(1, 2) * x, x / 2);

    for (size_t j = 0; j < sizeof(elements) / sizeof(elements[0]); j++) {
      if (i == j)
        EXPECT_EQ((x + x) / elements[j], 2);
      else
        EXPECT_EQ((x + x) / elements[j], std::optional<renf_elem_class>{});
    }
  }
}

TEST(ElementNF, Coefficients) {
  auto K = renf_class::make("a^2 - 2", "a", "1.41 +/- 0.1", 64);
  auto m = Module<NumberField>::make({RealNumber::rational(1), RealNumber::random()}, K);

  auto x = m->gen(1);
  auto a = renf_elem_class(K, "a");

  EXPECT_EQ(x.coefficients(), std::vector<renf_elem_class>({0, 1}));
  EXPECT_EQ((a * x).coefficients(), std::vector<renf_elem_class>({0, a}));
  EXPECT_EQ(x.coefficients<mpq_class>(), std::vector<mpq_class>({0, 0, 1, 0}));
  EXPECT_EQ((a * x).coefficients<mpq_class>(), std::vector<mpq_class>({0, 0, 0, 1}));
}
}  // namespace exactreal

#include "main.hpp"
