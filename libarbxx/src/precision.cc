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

#include "../arbxx/precision.hpp"

namespace arbxx {

namespace {

thread_local std::stack<prec> precisions;

}

Precision::Precision(prec prec) {
  precisions.push(prec);
}

Precision::~Precision() {
  precisions.pop();
}

prec Precision::current() {
  if (precisions.empty())
    throw std::logic_error("No precision has been specified in this scope.");
  return precisions.top();
}

}
