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

#ifndef LIBARBXX_PRECISION_HPP
#define LIBARBXX_PRECISION_HPP

#include "local.hpp"

namespace arbxx {

/// Sets a precision in the current scope.
/// This precision can then be used by operations that require a precision
/// to be set such as arithmetic of [Arb]() numbers.
///
///     #include <arbxx/precision.hpp>
///     #include <arbxx/arb.hpp>
///
///     arbxx::Precision prec{32};
///
///     arbxx::Arb a{1}, b{2};
///     a + b
///     // -> 3.00000
///
/// When the precision object goes out of scope, i.e., when its constructor is
/// called, the precision is reset to the previous value. Note that objects
/// must be destructed in the same order that they were constructed.
class LIBARBXX_API Precision {
 public:
  /// Set a precision for the lifetime of this object in the current thread.
  Precision(prec);

  ~Precision();

  /// Return the precision set in the current scope.
  static prec current();
};

}

#endif
