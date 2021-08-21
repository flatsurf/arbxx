/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2021 Julian Rüth
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

// TODO: Explain

#ifndef LIBEXACTREAL_LEGACY_1_4_0_ELEMENT_HPP
#define LIBEXACTREAL_LEGACY_1_4_0_ELEMENT_HPP

#include <vector>
#include <gmpxx.h>

#include "../../../exact-real/forward.hpp"

namespace exactreal {

template <typename Ring, typename C>
std::vector<C> Element_coefficients_1_4_0(const Element<Ring>&);

template <typename Ring, typename T>
Element<Ring>& Element_operator_mul_1_4_0(Element<Ring>&, const T&);

template <typename Ring, typename T>
Element<Ring>& Element_operator_div_1_4_0(Element<Ring>&, const T&);

}

#endif
