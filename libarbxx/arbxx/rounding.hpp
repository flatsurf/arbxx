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

#ifndef LIBARBXX_ROUNDING_HPP
#define LIBARBXX_ROUNDING_HPP

#include "local.hpp"

namespace arbxx {

/// Sets a rounding direction in the current scope.
/// This rounding can then be used by operations that require a rounding / to
//be specified such as arithmetic of [Arf]() numbers.
///
///     #include <arbxx/precision.hpp>
///     #include <arbxx/rounding.hpp>
///     #include <arbxx/arb.hpp>
///
///     arbxx::Precision prec{32};
///
///     arbxx::Arf a{1}, b{3};
///
///     arbxx::Rounding rounding{ARF_RND_DOWN};
///     a / b * b
///     // -> 1=4294967295p-32
///
///     arbxx::Rounding rounding{ARF_RND_UP};
///     a / b * b
///     // -> 1=2147483649p-31
///
///     arbxx::Rounding rounding{ARF_RND_NEAR};
///     a / b * b
///     // -> 1
///
/// When the object goes out of scope, i.e., when its destructor is
/// called, the rounding direction is reset to the previous value. Note that
//objects / must be destructed in the same order that they were constructed.
class LIBARBXX_API Rounding {
 public:
  /// Set a rounding direction for the lifetime of this object in the current thread.
  /// The parameter must be one of [ARF_RND_DOWN](), [ARF_RND_UP](),
  /// [ARF_RND_FLOOR](), [ARF_RND_CEIL](), [ARF_RND_NEAR]().
  Rounding(int);

  ~Rounding();

  /// Return the rounding direction for the current scope.
  static int current();
};

}

#endif

