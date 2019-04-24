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

#ifndef LIBEXACTREAL_YAP_FORWARD_HPP
#define LIBEXACTREAL_YAP_FORWARD_HPP

#include <boost/hana/fwd/all.hpp>
#include <boost/yap/expression.hpp>

namespace exactreal {
namespace yap {
template <boost::yap::expr_kind Kind, typename Tuple>
struct ArbExpr;

template <boost::yap::expr_kind Kind, typename Tuple>
struct ArfExpr;

struct PrecExpr;

struct RoundExpr;

struct PrecTransformation;
struct RoundTransformation;

template <bool precisionBound = false>
struct ArbAssignTransformation;

template <bool precisionBound = false, bool roundBound = false>
struct ArfAssignTransformation;
}  // namespace yap
}  // namespace exactreal

#endif
