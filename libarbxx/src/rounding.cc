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

#include <stack>
#include <stdexcept>

#include "../arbxx/rounding.hpp"

namespace arbxx {

namespace {

thread_local std::stack<int> roundings;

}

Rounding::Rounding(int rounding) {
  switch (rounding) {
    case ARF_RND_DOWN:
    case ARF_RND_UP:
    case ARF_RND_FLOOR:
    case ARF_RND_CEIL:
    case ARF_RND_NEAR:
      break;
    default:
      throw std::logic_error("Unsupported rounding direction.");
  }

  roundings.push(rounding);
}

Rounding::~Rounding() {
  roundings.pop();
}

int Rounding::current() {
  if (roundings.empty())
    throw std::logic_error("No rounding has been specified in this scope.");
  return roundings.top();
}

}
