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

#include <e-antic/renfxx.h>

#include "exact-real/number_field.hpp"
#include "exact-real/arb.hpp"

namespace exactreal {
Arb NumberField::arb(const ElementClass& x, mp_limb_signed_t prec) { return Arb(x, prec); }

bool NumberField::operator==(const NumberField& rhs) const { return parameters == rhs.parameters; }

NumberField::NumberField(const std::shared_ptr<const eantic::renf_class>& parameters) : parameters(parameters) {
  assert(parameters && "number field must not be null");
}

NumberField::NumberField() : NumberField(eantic::renf_class::make()) {}
}  // namespace exactreal
