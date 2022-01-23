/**********************************************************************
 *  This file is part of arbxx.
 *
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

#ifndef LIBARBXX_CPPYY_HPP
#define LIBARBXX_CPPYY_HPP

#include <boost/type_traits/is_detected.hpp>
#include <iosfwd>
#include <sstream>
#include <optional>

#include "arf.hpp"
#include "arb.hpp"

// See https://bitbucket.org/wlav/cppyy/issues/95/lookup-of-friend-operator
namespace arbxx {
std::ostream &operator<<(std::ostream &, const arbxx::Arb &);
std::ostream &operator<<(std::ostream &, const arbxx::Arf &);

namespace cppyy {

// cppyy has trouble looking up very complicated operators so we provide
// some stubs that help it get on the right track:
template <typename T>
Arb eval(T expression, prec prec) {
  Arb ret = std::move(expression)(prec);
  return ret;
}

template <typename T>
Arf eval(T expression, prec prec, Arf::Round round) {
  Arf ret;
  ret = std::move(expression)(prec, round);
  return ret;
}

template <typename S, typename T>
using truediv_t = decltype(std::declval<S>().truediv(std::declval<T>()));

template <typename S, typename T>
static constexpr bool has_truediv = boost::is_detected_v<truediv_t, S, T>;

// cppyy does not see the operators provided by boost::operators so we provide
// something to make them explicit here:
template <typename S, typename T>
auto add(const S &lhs, const T &rhs) { return lhs + rhs; }

template <typename S, typename T>
auto sub(const S &lhs, const T &rhs) { return lhs - rhs; }

template <typename S, typename T>
auto mul(const S &lhs, const T &rhs) { return lhs * rhs; }

template <typename S, typename T>
auto truediv(const S &lhs, const T &rhs) {
  if constexpr (has_truediv<const S &, const T &>) {
    return lhs.truediv(rhs);
  } else {
    return lhs / rhs;
  }
}

template <typename T>
auto neg(const T &value) { return -value; }

// TODO: Report upstream. cppyy casts the result of a relational operator to
// bool, i.e., it casts optional<bool> to bool which is just has_value.
template <typename S, typename T>
auto eq(const S& lhs, const T& rhs) { return lhs == rhs; }

template <typename S, typename T>
auto ne(const S& lhs, const T& rhs) { return lhs != rhs; }

template <typename S, typename T>
auto gt(const S& lhs, const T& rhs) { return lhs > rhs; }

template <typename S, typename T>
auto ge(const S& lhs, const T& rhs) { return lhs >= rhs; }

template <typename S, typename T>
auto lt(const S& lhs, const T& rhs) { return lhs < rhs; }

template <typename S, typename T>
auto le(const S& lhs, const T& rhs) { return lhs <= rhs; }

}  // namespace cppyy

}  // namespace arbxx

#endif
