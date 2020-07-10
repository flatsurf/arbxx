/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2019 Vincent Delecroix
 *        Copyright (C) 2019 Julian Rüth
 *
 *  exact-real is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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

#ifndef LIBEXACTREAL_NUMBER_FIELD_HPP
#define LIBEXACTREAL_NUMBER_FIELD_HPP

#include <optional>

#include <gmpxx.h>
#include <memory>

#include <e-antic/renfxx_fwd.h>
#include <boost/operators.hpp>

#include "forward.hpp"

namespace exactreal {

class NumberField : boost::equality_comparable<NumberField> {
 public:
  NumberField();
  NumberField(const std::shared_ptr<const eantic::renf_class>&);
  NumberField(const eantic::renf_elem_class&);

  std::shared_ptr<const eantic::renf_class> parameters;

  static NumberField compositum(const NumberField& lhs, const NumberField& rhs);

  bool operator==(const NumberField&) const;

  typedef eantic::renf_elem_class ElementClass;
  static constexpr bool isField = true;
  static bool unit(const ElementClass& x);
  static mpz_class floor(const ElementClass& x);
  static Arb arb(const ElementClass& x, long prec);
  static std::optional<mpq_class> rational(const ElementClass& x);
};

}  // namespace exactreal

#endif
