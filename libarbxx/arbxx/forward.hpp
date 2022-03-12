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

/*
 * Forward declarations of all our classes. Loaded by every other header file
 * for convenience and also to get consistent defaults for template parameters.
 */

#ifndef LIBARBXXL_FORWARD_HPP
#define LIBARBXXL_FORWARD_HPP

#include "local.hpp"

namespace arbxx {
class Precision;
class Rounding;

class Mag;
class Arf;
class Arb;
class Acb;
class ArbPoly;
class AcbPoly;
class ArbMat;
class AcbMat;

}  // namespace arbxx

#endif
