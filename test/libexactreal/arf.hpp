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

#ifndef EXACTREAL_TEST_ARF_HPP
#define EXACTREAL_TEST_ARF_HPP

#include <flint/flintxx/frandxx.h>
#include <cassert>
#include <exact-real/arf.hpp>
#include <memory>

using namespace exactreal;
using std::make_unique;
using std::unique_ptr;

struct ArfTester {
  unique_ptr<flint::frandxx> flint_rand;

  void reset() { flint_rand = make_unique<flint::frandxx>(); }

  Arf random(prec prec = 53, size mag = 10) {
    assert(prec != 0);
    return Arf::randtest(*flint_rand, prec, mag);
  }
};

#endif
