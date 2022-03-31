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

/// C++ Wrappers for a floating point number [arf_t]().

#ifndef LIBARBXX_ARF_HPP
#define LIBARBXX_ARF_HPP

#include <arf.h>
#include <flint/flintxx/frandxx.h>
#include <gmpxx.h>

#include <boost/operators.hpp>
#include <memory>

#include "local.hpp"

namespace arbxx {

/// A wrapper for [arf_t]() elements, i.e., arbitrary precision floating point
/// numbers, providing C++ style memory management.
///
///     #include <arbxx/arf.hpp>
///     arbxx::Arf x, y;
///
/// Instances of this class can be used as arguments in the C API of Arb.
///
///     arf_add(x, x, y, 64, ARF_RND_NEAR);
///
/// Some parts of the C API that naturally translate to class members, have
/// been implemented, see belowe. However, arithmetic operators are not readily
/// available.
///
///     x + y
///     // -> No rounding has been specified in this scope.
///
/// They only become available once a precision and a rounding direction have been fixed.
///
///     #include <arbxx/precision.hpp>
///     #include <arbxx/rounding.hpp>
///
///     arbxx::Precision prec{64};
///     arbxx::Rounding round{ARF_RND_NEAR};
///
///     x + y
///     // -> 0
///
/// Note that these operators are less efficient than calling the corresponding
/// C function from Arb directly. (See comments below.)
class LIBARBXX_API Arf : boost::arithmetic<Arf>,
                         boost::totally_ordered<Arf>,
                         boost::totally_ordered<Arf, short>,
                         boost::totally_ordered<Arf, unsigned short>,
                         boost::totally_ordered<Arf, int>,
                         boost::totally_ordered<Arf, unsigned int>,
                         boost::totally_ordered<Arf, long>,
                         boost::totally_ordered<Arf, unsigned long>,
                         boost::totally_ordered<Arf, long long>,
                         boost::totally_ordered<Arf, unsigned long long>,
                         boost::totally_ordered<Arf, const mpz_class&>,
                         boost::shiftable<Arf, long> {
 public:
  /// Create a zero element.
  ///
  ///     arbxx::Arf x;
  ///     x
  ///     // -> 0
  ///
  inline Arf() noexcept { arf_init(*this); }

  /// Create a copy of an element.
  ///
  ///     arbxx::Arf x{1};
  ///     arbxx::Arf y{x};
  ///     y
  ///     // -> 1
  ///
  inline Arf(const Arf& rhs) noexcept: Arf() { arf_set(*this, rhs); }

  /// Create an element from another one.
  ///
  ///     arbxx::Arf x{1};
  ///     arbxx::Arf y{std::move(x)};
  ///     y
  ///     // -> 1
  ///
  /// The original element `x` is moved into the newly created element. The
  /// original element should not be used anymore after that. (Currently, it's
  /// just reset to zero but that might change in the future.)
  Arf(Arf&& rhs) noexcept: Arf() { swap(*this, rhs); }

  /// Create the element `mantissa * 2^exponent` where `mantissa` is an integer
  /// written in `base`.
  ///
  ///     arbxx::Arf x{"abc", 16, -8};
  ///     x
  ///     // -> 10.7344=687p-6
  ///
  Arf(const std::string& mantissa, int base, long exponent);

  /// Create the element `mantissa * 2^exponent`.
  ///
  ///     arbxx::Arf x{2748, -8};
  ///     std::cout << x;
  ///     // -> 10.7344=687p-6
  ///
  explicit Arf(const mpz_class& mantissa, long exponent = 0);

  /// ==* Arf(integer) *==
  /// Create a floating point number from an integer.
  ///
  ///     arbxx::Arf x{2748};
  ///     std::cout << x;
  ///     // -> 2748
  ///
  explicit Arf(short value) : Arf(static_cast<long>(value)) {}
  explicit Arf(unsigned short value) : Arf(static_cast<unsigned long>(value)) {}
  explicit Arf(int value) : Arf(static_cast<long>(value)) {}
  explicit Arf(unsigned int value) : Arf(static_cast<unsigned long>(value)) {}
  explicit Arf(long value) { arf_init_set_si(*this, value); }
  explicit Arf(unsigned long value) { arf_init_set_ui(*this, value) ;}
  explicit Arf(long long);
  explicit Arf(unsigned long long);

  /// Create a floating point element identical to this double.
  ///
  ///     arbxx::Arf x{0.3};
  ///     std::cout << x;
  ///     // -> 0.3=5404319552844595p-54
  ///
  explicit Arf(double);

  inline ~Arf() noexcept { arf_clear(*this); }

  /// Make this element an identical copy of the provided one.
  ///
  ///     arbxx::Arf x{1}, y;
  ///     y = x;
  ///     std::cout << y;
  ///     // -> 1
  ///
  Arf& operator=(const Arf&) noexcept;

  /// Move the value of the element into this one.
  ///
  ///     arbxx::Arf x{1}, y;
  ///     y = std::move(x);
  ///     std::cout << y;
  ///     // -> 1
  ///
  Arf& operator=(Arf&&) noexcept;

  /// ==* operator=(integer) *==
  /// Reset the value to this integer.
  ///
  ///     arbxx::Arf x;
  ///     x = 1;
  ///     std::cout << x;
  ///     // -> 1
  ///
  Arf& operator=(short);
  Arf& operator=(unsigned short);
  Arf& operator=(int);
  Arf& operator=(unsigned);
  Arf& operator=(long);
  Arf& operator=(unsigned long);
  Arf& operator=(long long);
  Arf& operator=(unsigned long long);
  Arf& operator=(const mpz_class&);

  /// Reset the value to the value of this double.
  ///
  ///     arbxx::Arf x;
  ///     x = 0.3;
  ///     std::cout << x;
  ///     // -> 0.3=5404319552844595p-54
  ///
  Arf& operator=(double);

  /// Return the negative of this value, see [arf_neg]().
  ///
  ///     arbxx::Arf x{1};
  ///     arbxx::Arf y = -x;
  ///     std::cout << y;
  ///     // -> -1
  ///
  Arf operator-() const;

  /// Return the double precision floating point number closest to this
  /// element, see [arf_get_d]().
  ///
  ///     arbxx::Arf x{1, -65536};
  ///     std::cout << static_cast<double>(x);
  ///     // -> 0
  ///
  ///     #include <arbxx/precision.hpp>
  ///     #include <arbxx/rounding.hpp>
  ///
  ///     arbxx::Precision prec{64};
  ///     arbxx::Rounding round{ARF_RND_NEAR};
  ///
  ///     x *= arbxx::Arf{1, 65536};
  ///     std::cout << static_cast<double>(x);
  ///     // -> 1
  ///
  explicit operator double() const;

  /// Return the absolute value of this element, see [arf_abs]().
  ///
  ///     arbxx::Arf x{-1};
  ///     arbxx::Arf y = x.abs();
  ///     std::cout << y;
  ///     // -> 1
  ///
  Arf abs() const;

  /// Return the largest integer which is less than or equal to this element,
  /// see [arf_floor]().
  ///
  ///     arbxx::Arf x{-.5};
  ///     std::cout << x.floor();
  ///     // -> -1
  ///
  mpz_class floor() const;

  /// Return the largest integer which is greater than or equal to this
  /// element, see [arf_ceil]().
  ///
  ///     arbxx::Arf x{-.5};
  ///     std::cout << x.ceil();
  ///     // -> 0
  ///
  mpz_class ceil() const;

  /// Return the mantissa of this element, i.e., return `mantissa` when writing
  /// this element as `mantissa * 2^exponent`. See [arf_get_fmpz_2exp]().
  ///
  ///     arbxx::Arf x{-1};
  ///     std::cout << x.mantissa();
  ///     // -> -1
  ///
  mpz_class mantissa() const;

  /// Return the exponent of this element, i.e., return `mantissa` when writing
  /// this element as `mantissa * 2^exponent`. See [arf_get_fmpz_2exp]().
  ///
  ///     arbxx::Arf x{-1};
  ///     std::cout << x.exponent();
  ///     // -> 0
  ///
  mpz_class exponent() const;

  /// Return the floor of the logarithm of this element to base 2.
  ///
  ///     arbxx::Arf x{1025};
  ///     x.logb()
  ///     // -> 10
  ///
  long logb() const;

  /// Return this element shifted down by `e`, i.e., multiplied by `2^-e`.
  ///
  ///     arbxx::Arf x{-1};
  ///     x >>= 8;
  ///     std::cout << x;
  ///     // -> -0.00390625=-1p-8
  ///
  Arf& operator>>=(const long e);

  /// Return this element shifted up by `e`, i.e., multiplied by `2^e`.
  ///
  ///     arbxx::Arf x{-1};
  ///     x <<= 8;
  ///     std::cout << x;
  ///     // -> -256
  ///
  Arf& operator<<=(const long e);

  /// ==* Comparison Operators *==
  /// Comparison operators are available for comparison with other `Arf`
  /// instances and integer types.
  ///
  ///     arbxx::Arf x{1};
  ///     x < 2
  ///     // -> true
  ///
  ///     x <= mpz_class{1}
  ///     // -> true
  ///
  LIBARBXX_API friend bool operator<(const Arf&, const Arf&);
  LIBARBXX_API friend bool operator==(const Arf&, const Arf&);

  LIBARBXX_API friend bool operator<(const Arf&, short);
  LIBARBXX_API friend bool operator>(const Arf&, short);
  LIBARBXX_API friend bool operator==(const Arf&, short);

  LIBARBXX_API friend bool operator<(const Arf&, unsigned short);
  LIBARBXX_API friend bool operator>(const Arf&, unsigned short);
  LIBARBXX_API friend bool operator==(const Arf&, unsigned short);

  LIBARBXX_API friend bool operator<(const Arf&, int);
  LIBARBXX_API friend bool operator>(const Arf&, int);
  LIBARBXX_API friend bool operator==(const Arf&, int);

  LIBARBXX_API friend bool operator<(const Arf&, unsigned int);
  LIBARBXX_API friend bool operator>(const Arf&, unsigned int);
  LIBARBXX_API friend bool operator==(const Arf&, unsigned int);

  LIBARBXX_API friend bool operator<(const Arf&, long);
  LIBARBXX_API friend bool operator>(const Arf&, long);
  LIBARBXX_API friend bool operator==(const Arf&, long);

  LIBARBXX_API friend bool operator<(const Arf&, unsigned long);
  LIBARBXX_API friend bool operator>(const Arf&, unsigned long);
  LIBARBXX_API friend bool operator==(const Arf&, unsigned long);

  LIBARBXX_API friend bool operator<(const Arf&, long long);
  LIBARBXX_API friend bool operator>(const Arf&, long long);
  LIBARBXX_API friend bool operator==(const Arf&, long long);

  LIBARBXX_API friend bool operator<(const Arf&, unsigned long long);
  LIBARBXX_API friend bool operator>(const Arf&, unsigned long long);
  LIBARBXX_API friend bool operator==(const Arf&, unsigned long long);

  LIBARBXX_API friend bool operator<(const Arf&, const mpz_class&);
  LIBARBXX_API friend bool operator>(const Arf&, const mpz_class&);
  LIBARBXX_API friend bool operator==(const Arf&, const mpz_class&);

  /// The binary operators `+`, `-`, `*`, `/` are available.
  ///
  ///     #include <arbxx/precision.hpp>
  ///     #include <arbxx/rounding.hpp>
  ///
  ///     arbxx::Precision prec{64};
  ///     arbxx::Rounding round{ARF_RND_NEAR};
  ///
  ///     arbxx::Arf x{1}, y{2};
  ///
  ///     x + y
  ///     // -> 3
  ///
  ///     x - y
  ///     // -> -1
  ///
  ///     x * y
  ///     // -> 2
  ///
  ///     x / y
  ///     // -> 0.5=1p-1
  ///
  /// For small operands, these operators can be much slower (factor 2) than
  /// using the corresponding C API directly, i.e., [arf_add](), [arf_sub](),
  /// [arf_mul](), [arf_div](). This is essentially due to call overhead and
  /// also because an additional temporary is needed.
  ///
  /// There are also inplace operators that are slightly faster because they do
  /// not need the temporary, but still the call overhead can be substantial.
  ///
  ///     x += y;
  ///     x -= y;
  ///     x *= y;
  ///     x /= y;
  ///
  LIBARBXX_API friend Arf& operator+=(Arf&, const Arf&);
  LIBARBXX_API friend Arf& operator-=(Arf&, const Arf&);
  LIBARBXX_API friend Arf& operator*=(Arf&, const Arf&);
  LIBARBXX_API friend Arf& operator/=(Arf&, const Arf&);

  /// Return a random element, see [arf_randtest]().
  ///
  ///     #include <flint/flintxx/frandxx.h>
  ///
  ///     flint::frandxx rand;
  ///     auto a = arbxx::Arf::randtest(rand, 64, 16);
  ///     auto b = arbxx::Arf::randtest(rand, 64, 16);
  ///     a == b
  ///     // -> false
  ///
  LIBARBXX_API static Arf randtest(flint::frandxx&, prec precision, prec magbits);

  /// Write this element to the output stream.
  ///
  ///     arbxx::Arf x{1};
  ///     std::cout << x;
  ///     // -> 1
  ///
  /// Non-integers and large numbers are written as their double approximation
  /// and exactly, with an exponent for base two, here is for example `2^-16`:
  ///
  ///     arbxx::Arf x{1, -16};
  ///     std::cout << x;
  ///     // -> 1.52588e-05=1p-16
  ///
  LIBARBXX_API friend std::ostream& operator<<(std::ostream&, const Arf&);

  /// Swap two elements efficiently.
  /// This is used by some STL containers.
  ///
  ///     arbxx::Arf x{1}, y;
  ///     std::cout << "x = " << x << ", y = " << y;
  ///     // -> x = 1, y = 0
  ///
  ///     swap(x, y);
  ///     std::cout << "x = " << x << ", y = " << y;
  ///     // -> x = 0, y = 1
  ///
  LIBARBXX_API friend inline void swap(Arf& lhs, Arf& rhs) {
    arf_swap(lhs, rhs);
  }

  /// Return a reference to the underlying [arf_t]() element for direct
  /// manipulation with the C API of Arb.
  ///
  ///     arbxx::Arf x;
  ///     arf_set_ui(x.arf_t(), 1);
  ///     x
  ///     // -> 1
  ///
  inline ::arf_t& arf_t() { return *this; }

  /// Implicitly cast this element to the underlying [::arf_t]() for direct
  /// manipulation with the C API of Arb.
  ///
  ///     arbxx::Arf x;
  ///     arf_set_ui(x, 1);
  ///     x
  ///     // -> 1
  ///
  inline operator ::arf_t&() { return t; }

  /// Return a reference to the underlying [::arf_t]() element for direct
  /// interaction with the C API of Arb.
  ///
  ///     arbxx::Arf x{1};
  ///     arf_get_d(x.arf_t(), ARF_RND_NEAR)
  ///     // -> 1
  ///
  inline const ::arf_t& arf_t() const { return *this; }

  /// Implicitly cast this element to the underlying [::arf_t() for direct
  /// interaction with the C API of Arb.
  ///
  ///     arbxx::Arf x{1};
  ///     arf_get_d(x, ARF_RND_NEAR)
  ///     // -> 1
  ///
  inline operator const ::arf_t&() const { return t; }

 private:
  /// The underlying [::arf_t](); use [arf_t()]() to get a reference to it.
  ::arf_t t;
};

}  // namespace arbxx

namespace std {

template <>
struct LIBARBXX_API hash<arbxx::Arf> {
  size_t LIBARBXX_API operator()(const arbxx::Arf&) const;
};

}  // namespace std

#endif
