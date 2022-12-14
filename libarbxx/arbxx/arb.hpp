/* ********************************************************************
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
 * *******************************************************************/

/// C++ Wrappers for a ball arithmetic [arb_t]().

#ifndef LIBARBXX_ARB_HPP
#define LIBARBXX_ARB_HPP

#include <arb.h>
#include <flint/flintxx/frandxx.h>
#include <gmpxx.h>

#include <memory>
#include <optional>

#include "arf.hpp"

namespace arbxx {

/// A wrapper for [::arb_t]() elements, i.e., floating point numbers surrounded
/// by a real ball of imprecision, providing C++ style memory management.
/// 
///     #include <arbxx/arb.hpp>
///     arbxx::Arb x, y;
///
/// Instances of this class can be used as arguments in the C API of Arb.
///
///     arb_add(x, x, y, 64);
///
/// Some parts of the C API that naturally translate to class members, have
/// been implemented, see below. However, arithmetic operators are not readily
/// available.
///
///     x * y
///     // -> No precision has been specified in this scope.
///
/// They only become available once a precision has been fixed.
///
///     #include <arbxx/precision.hpp>
///
///     arbxx::Precision prec{64};
///
///     x * y
///     // -> 0
///
/// Note that these operators are less efficient than calling the corresponding
/// C function from Arb directly. (See comments below.)
class LIBARBXX_API Arb : boost::arithmetic<Arb>,
                         boost::arithmetic<Arb, Arf>,
                         boost::multipliable<Arb, short>,
                         boost::multipliable<Arb, unsigned short>,
                         boost::multipliable<Arb, int>,
                         boost::multipliable<Arb, unsigned int>,
                         boost::multipliable<Arb, long>,
                         boost::multipliable<Arb, unsigned long>,
                         boost::multipliable<Arb, long long>,
                         boost::multipliable<Arb, unsigned long long>,
                         boost::multipliable<Arb, mpz_class> {
 public:
  /// Create an exact zero element.
  ///
  ///     arbxx::Arb x;
  ///     x
  ///     // -> 0
  ///
  Arb() noexcept;

  /// Create a copy of `x`.
  ///
  ///     arbxx::Arb x{1337};
  ///     arbxx::Arb y{x};
  ///     (x == y) == true
  ///     // -> true
  ///
  Arb(const Arb&) noexcept;

  /// Create a new element from `x`.
  ///
  ///     arbxx::Arb x{1337};
  ///     arbxx::Arb y{std::move(x)};
  ///     std::cout << y;
  ///     // -> 1337.00
  ///
  Arb(Arb&&) noexcept;

  /// Create an exact element equal to this integer.
  ///
  ///     arbxx::Arb x{mpz_class{1337}};
  ///     std::cout << x;
  ///     // -> 1337.00
  ///
  explicit Arb(const mpz_class&);

  /// Create an element containing this rational using [arb_set_fmpq](), i.e.,
  /// by performing the division of numerator and denominator with precision
  /// `prec`.
  ///
  ///     arbxx::Arb x{mpq_class{1, 2}, 256};
  ///     std::cout << x;
  ///     // -> 0.500000
  ///
  ///     arbxx::Arb y{mpq_class{1, 3}, 256};
  ///     std::cout << std::setprecision(32) << y;
  ///     // -> [0.33333333333333333333333333333333 +/- 3.34e-33]
  ///
  Arb(const mpq_class&, const prec);

  /// Create an Arb ball with lower and upper bound as given by the pair, see
  /// [arb_set_interval_arf]().
  ///
  ///     #include <arbxx/arf.hpp>
  ///
  ///     arbxx::Arf lower{0}, upper{1};
  ///     arbxx::Arb x{std::pair{lower, upper}};
  ///     std::cout << x;
  ///     // -> [0.500000 +/- 0.501]
  ///
  ///     arbxx::Arb y{std::pair{lower, upper}, 64};
  ///     std::cout << y;
  ///     // -> [0.500000 +/- 0.501]
  ///
  explicit Arb(const std::pair<Arf, Arf>&, const prec = ARF_PREC_EXACT);

  /// Create an exact element equal to the given floating point element, see
  /// [arb_set_arf]().
  ///
  ///     #include <arbxx/arf.hpp>
  ///
  ///     arbxx::Arf x{1};
  ///     arbxx::Arb y{x};
  ///     std::cout << y;
  ///     // -> 1.00000
  ///
  explicit Arb(const Arf&);

  /// ==* `Arb(integer)` *==
  /// Create an exact element, equal to this integer.
  ///
  ///     arbxx::Arb x{1};
  ///     std::cout << x;
  ///     // -> 1.00000
  ///
  explicit Arb(short);
  explicit Arb(unsigned short);
  explicit Arb(int);
  explicit Arb(unsigned int);
  explicit Arb(long);
  explicit Arb(unsigned long);
  explicit Arb(long long);
  explicit Arb(unsigned long long);

  /// Create an element from this string, see [arb_set_str]().
  ///
  ///     arbxx::Arb x{"[3.25 +/- 0.0001]", 64};
  ///     std::cout << x;
  ///     // -> [3.25000 +/- 1.01e-4]
  ///
  /// Note that, as in the above example, the radius is not guaranteed to be
  /// preserved when going from an element to its string representation and
  /// back. See https://github.com/fredrik-johansson/arb/issues/412.
  Arb(const std::string&, const prec);

  /// TODO
  ~Arb() noexcept;

  /// ==* `operator=(Arb)` *==
  /// Reset this element to the one given.
  ///
  ///     arbxx::Arb x{1}, y;
  ///     y = std::move(x);
  ///     std::cout << y;
  ///     // -> 1.00000
  ///
  ///     x = y;
  ///     std::cout << y;
  ///     // -> 1.00000
  ///
  Arb& operator=(const Arb&) noexcept;
  Arb& operator=(Arb&&) noexcept;

  /// ==* `operator=(integer)` *==
  /// Reset this elemen to an exact element representing this integer.
  ///
  ///     arbxx::Arb x;
  ///     x = 1;
  ///     std::cout << x;
  ///     // -> 1.00000
  ///
  Arb& operator=(short);
  Arb& operator=(unsigned short);
  Arb& operator=(int);
  Arb& operator=(unsigned int);
  Arb& operator=(long);
  Arb& operator=(unsigned long);
  Arb& operator=(long long);
  Arb& operator=(unsigned long long);
  Arb& operator=(const mpz_class&);

  /// Return the negative of this element.
  /// This method returns a ball whose lower and upper bound is the negative of
  /// the upper and lower bound, respectively.
  ///
  ///     arbxx::Arb x{1};
  ///     std::cout << -x;
  ///     // -> -1.00000
  ///
  Arb operator-() const;

  /// ==* Comparison Operators *==
  /// The comparison operators return a value if the relation is true for all
  /// elements in the ball described by the element, e.g., x < y, returns true
  /// if the relation is true for every element in x and y, they return false if
  /// the relation is false for every element in x and y, and nothing otherwise.
  /// Note that this is different from the semantic in Arb where false is
  /// returned in both of the latter cases.
  ///
  ///     arbxx::Arb x{mpq_class{1, 3}, 64};
  ///     (x < 1).has_value()
  ///     // -> true
  ///
  ///     (x < 1).value()
  ///     // -> true
  ///
  ///     (x < x).has_value()
  ///     // -> false
  ///
  LIBARBXX_API friend std::optional<bool> operator==(const Arb&, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator!=(const Arb&, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator<(const Arb&, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator>(const Arb&, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator<=(const Arb&, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator>=(const Arb&, const Arb&);

  LIBARBXX_API friend std::optional<bool> operator==(const Arb&, short);
  LIBARBXX_API friend std::optional<bool> operator!=(const Arb&, short);
  LIBARBXX_API friend std::optional<bool> operator<(const Arb&, short);
  LIBARBXX_API friend std::optional<bool> operator>(const Arb&, short);
  LIBARBXX_API friend std::optional<bool> operator<=(const Arb&, short);
  LIBARBXX_API friend std::optional<bool> operator>=(const Arb&, short);
  LIBARBXX_API friend std::optional<bool> operator==(short, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator!=(short, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator<(short, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator>(short, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator<=(short, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator>=(short, const Arb&);

  LIBARBXX_API friend std::optional<bool> operator==(const Arb&, unsigned short);
  LIBARBXX_API friend std::optional<bool> operator!=(const Arb&, unsigned short);
  LIBARBXX_API friend std::optional<bool> operator<(const Arb&, unsigned short);
  LIBARBXX_API friend std::optional<bool> operator>(const Arb&, unsigned short);
  LIBARBXX_API friend std::optional<bool> operator<=(const Arb&, unsigned short);
  LIBARBXX_API friend std::optional<bool> operator>=(const Arb&, unsigned short);
  LIBARBXX_API friend std::optional<bool> operator==(unsigned short, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator!=(unsigned short, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator<(unsigned short, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator>(unsigned short, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator<=(unsigned short, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator>=(unsigned short, const Arb&);

  LIBARBXX_API friend std::optional<bool> operator==(const Arb&, int);
  LIBARBXX_API friend std::optional<bool> operator!=(const Arb&, int);
  LIBARBXX_API friend std::optional<bool> operator<(const Arb&, int);
  LIBARBXX_API friend std::optional<bool> operator>(const Arb&, int);
  LIBARBXX_API friend std::optional<bool> operator<=(const Arb&, int);
  LIBARBXX_API friend std::optional<bool> operator>=(const Arb&, int);
  LIBARBXX_API friend std::optional<bool> operator==(int, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator!=(int, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator<(int, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator>(int, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator<=(int, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator>=(int, const Arb&);

  LIBARBXX_API friend std::optional<bool> operator==(const Arb&, unsigned int);
  LIBARBXX_API friend std::optional<bool> operator!=(const Arb&, unsigned int);
  LIBARBXX_API friend std::optional<bool> operator<(const Arb&, unsigned int);
  LIBARBXX_API friend std::optional<bool> operator>(const Arb&, unsigned int);
  LIBARBXX_API friend std::optional<bool> operator<=(const Arb&, unsigned int);
  LIBARBXX_API friend std::optional<bool> operator>=(const Arb&, unsigned int);
  LIBARBXX_API friend std::optional<bool> operator==(unsigned int, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator!=(unsigned int, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator<(unsigned int, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator>(unsigned int, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator<=(unsigned int, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator>=(unsigned int, const Arb&);

  LIBARBXX_API friend std::optional<bool> operator==(const Arb&, long);
  LIBARBXX_API friend std::optional<bool> operator!=(const Arb&, long);
  LIBARBXX_API friend std::optional<bool> operator<(const Arb&, long);
  LIBARBXX_API friend std::optional<bool> operator>(const Arb&, long);
  LIBARBXX_API friend std::optional<bool> operator<=(const Arb&, long);
  LIBARBXX_API friend std::optional<bool> operator>=(const Arb&, long);
  LIBARBXX_API friend std::optional<bool> operator==(long, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator!=(long, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator<(long, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator>(long, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator<=(long, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator>=(long, const Arb&);

  LIBARBXX_API friend std::optional<bool> operator==(const Arb&, unsigned long);
  LIBARBXX_API friend std::optional<bool> operator!=(const Arb&, unsigned long);
  LIBARBXX_API friend std::optional<bool> operator<(const Arb&, unsigned long);
  LIBARBXX_API friend std::optional<bool> operator>(const Arb&, unsigned long);
  LIBARBXX_API friend std::optional<bool> operator<=(const Arb&, unsigned long);
  LIBARBXX_API friend std::optional<bool> operator>=(const Arb&, unsigned long);
  LIBARBXX_API friend std::optional<bool> operator==(unsigned long, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator!=(unsigned long, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator<(unsigned long, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator>(unsigned long, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator<=(unsigned long, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator>=(unsigned long, const Arb&);

  LIBARBXX_API friend std::optional<bool> operator==(const Arb&, long long);
  LIBARBXX_API friend std::optional<bool> operator!=(const Arb&, long long);
  LIBARBXX_API friend std::optional<bool> operator<(const Arb&, long long);
  LIBARBXX_API friend std::optional<bool> operator>(const Arb&, long long);
  LIBARBXX_API friend std::optional<bool> operator<=(const Arb&, long long);
  LIBARBXX_API friend std::optional<bool> operator>=(const Arb&, long long);
  LIBARBXX_API friend std::optional<bool> operator==(long long, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator!=(long long, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator<(long long, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator>(long long, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator<=(long long, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator>=(long long, const Arb&);

  LIBARBXX_API friend std::optional<bool> operator==(const Arb&, unsigned long long);
  LIBARBXX_API friend std::optional<bool> operator!=(const Arb&, unsigned long long);
  LIBARBXX_API friend std::optional<bool> operator<(const Arb&, unsigned long long);
  LIBARBXX_API friend std::optional<bool> operator>(const Arb&, unsigned long long);
  LIBARBXX_API friend std::optional<bool> operator<=(const Arb&, unsigned long long);
  LIBARBXX_API friend std::optional<bool> operator>=(const Arb&, unsigned long long);
  LIBARBXX_API friend std::optional<bool> operator==(unsigned long long, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator!=(unsigned long long, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator<(unsigned long long, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator>(unsigned long long, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator<=(unsigned long long, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator>=(unsigned long long, const Arb&);

  LIBARBXX_API friend std::optional<bool> operator==(const Arb&, const mpz_class&);
  LIBARBXX_API friend std::optional<bool> operator!=(const Arb&, const mpz_class&);
  LIBARBXX_API friend std::optional<bool> operator<(const Arb&, const mpz_class&);
  LIBARBXX_API friend std::optional<bool> operator>(const Arb&, const mpz_class&);
  LIBARBXX_API friend std::optional<bool> operator<=(const Arb&, const mpz_class&);
  LIBARBXX_API friend std::optional<bool> operator>=(const Arb&, const mpz_class&);
  LIBARBXX_API friend std::optional<bool> operator==(const mpz_class&, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator!=(const mpz_class&, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator<(const mpz_class&, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator>(const mpz_class&, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator<=(const mpz_class&, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator>=(const mpz_class&, const Arb&);

  LIBARBXX_API friend std::optional<bool> operator==(const Arb&, const mpq_class&);
  LIBARBXX_API friend std::optional<bool> operator!=(const Arb&, const mpq_class&);
  LIBARBXX_API friend std::optional<bool> operator<(const Arb&, const mpq_class&);
  LIBARBXX_API friend std::optional<bool> operator>(const Arb&, const mpq_class&);
  LIBARBXX_API friend std::optional<bool> operator<=(const Arb&, const mpq_class&);
  LIBARBXX_API friend std::optional<bool> operator>=(const Arb&, const mpq_class&);
  LIBARBXX_API friend std::optional<bool> operator==(const mpq_class&, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator!=(const mpq_class&, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator<(const mpq_class&, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator>(const mpq_class&, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator<=(const mpq_class&, const Arb&);
  LIBARBXX_API friend std::optional<bool> operator>=(const mpq_class&, const Arb&);

  /// TODO
  LIBARBXX_API friend Arb& operator+=(Arb&, const Arb&);
  LIBARBXX_API friend Arb& operator-=(Arb&, const Arb&);
  LIBARBXX_API friend Arb& operator*=(Arb&, const Arb&);
  LIBARBXX_API friend Arb& operator/=(Arb&, const Arb&);

  /// TODO
  LIBARBXX_API friend Arb& operator+=(Arb&, const Arf&);
  LIBARBXX_API friend Arb& operator-=(Arb&, const Arf&);
  LIBARBXX_API friend Arb& operator*=(Arb&, const Arf&);
  LIBARBXX_API friend Arb& operator/=(Arb&, const Arf&);

  /// TODO
  LIBARBXX_API friend Arb& operator*=(Arb&, short);
  LIBARBXX_API friend Arb& operator*=(Arb&, unsigned short);
  LIBARBXX_API friend Arb& operator*=(Arb&, int);
  LIBARBXX_API friend Arb& operator*=(Arb&, unsigned int);
  LIBARBXX_API friend Arb& operator*=(Arb&, long);
  LIBARBXX_API friend Arb& operator*=(Arb&, unsigned long);
  LIBARBXX_API friend Arb& operator*=(Arb&, long long);
  LIBARBXX_API friend Arb& operator*=(Arb&, unsigned long long);
  LIBARBXX_API friend Arb& operator*=(Arb&, const mpz_class&);

  /// Return whether this Arb element exactly represents a floating point
  /// number, i.e., whether its radius is zero, see [arb_is_exact]().
  ///
  ///     arbxx::Arb x{1};
  ///     x.is_exact()
  ///     // -> true
  ///
  ///     arbxx::Arb y{mpq_class{1, 3}, 64};
  ///     y.is_exact()
  ///     // -> false
  ///
  bool is_exact() const;

  /// Return whether this Arb element does contain neither plus nor minus
  /// infinity, see [arb_is_finite]().
  ///
  ///     arbxx::Arb x{1};
  ///     x.is_finite()
  ///     // -> true
  ///
  ///     arbxx::Arb y{1};
  ///     arb_div_si(y.arb_t(), y.arb_t(), 0, 64);
  ///     y.is_finite()
  ///     // -> false
  ///
  bool is_finite() const;

  /// Return the lower and the upper bound of this ball.
  /// See [arb_get_interval_arf]().
  ///
  ///     arbxx::Arb x{mpq_class{1, 3}, 64};
  ///     auto bounds = static_cast<std::pair<arbxx::Arf, arbxx::Arf>>(x);
  ///     std::cout << bounds.first << ", " << bounds.second;
  ///     // -> 0.333333=1537228672809129301p-62, 0.333333=3074457345618258603p-63
  ///
  explicit operator std::pair<Arf, Arf>() const;

  /// Return a printable representation of this element.
  /// Note that parsing the string might not produce the same element, see
  /// https://github.com/fredrik-johansson/arb/issues/412.
  ///
  ///     arbxx::Arb x{mpq_class{1, 3}, 64};
  ///     static_cast<std::string>(x)
  ///     // -> [0.333333333333333333315263297125241592766542453318834304809570313 +/- 2.72e-20]
  ///
  explicit operator std::string() const;

  /// Return the midpoint of this ball rounded to the closest double.
  /// Note that ties are rounded to even.
  ///
  ///     arbxx::Arb x{mpq_class{1, 3}, 64};
  ///     static_cast<double>(x)
  ///     // -> 0.333333
  ///
  explicit operator double() const;

  /// Return the exact midpoint of this ball.
  ///
  ///     #include <arbxx/arf.hpp>
  ///
  ///     arbxx::Arb x{mpq_class{1, 3}, 64};
  ///     std::cout << static_cast<arbxx::Arf>(x);
  ///     // -> 0.333333=6148914691236517205p-64
  ///
  explicit operator Arf() const;

  /// Write this element to the output stream, see [arb_get_str]().
  LIBARBXX_API friend std::ostream& operator<<(std::ostream&, const Arb&);

  /// Return a reference to the underlying [arb_t]() element for direct
  /// manipulation with the C API of Arb.
  ::arb_t& arb_t();

  /// TODO
  inline operator ::arb_t&() {
    return t;
  }

  /// Return a const reference to the underlying [arb_t]() element for direct
  /// manipulation with the C API of Arb.
  const ::arb_t& arb_t() const;

  // TODO
  inline operator const ::arb_t&() const {
    return t;
  }

  /// Return an exact zero element, i.e., the ball of radius zero centered at
  /// zero.
  ///
  ///     std::cout << arbxx::Arb::zero();
  ///     // -> 0
  ///
  static Arb zero();

  /// Return an exact one element, i.e., the ball of radius zero centered at
  /// one.
  ///
  ///     std::cout << arbxx::Arb::one();
  ///     // -> 1.00000
  ///
  static Arb one();

  /// Return plus infinity, i.e., the ball of radius zero centered at plus
  /// infinity, see [arb_pos_inf]().
  /// Note that the result is printed as `[+/- inf]` unfortunately, see
  /// https://github.com/fredrik-johansson/arb/issues/332.
  ///
  ///     std::cout << arbxx::Arb::pos_inf();
  ///     // -> [+/- inf]
  ///
  static Arb pos_inf();

  /// Return minus infinity, i.e., the ball of radius zero centered at minus
  /// infinity, see [arb_neg_inf]().
  /// Note that the result is printed as `[+/- inf]` unfortunately, see
  /// https://github.com/fredrik-johansson/arb/issues/332.
  ///
  ///     std::cout << arbxx::Arb::neg_inf();
  ///     // -> [+/- inf]
  ///
  static Arb neg_inf();

  /// Return the extended real line, i.e., the interval [-∞,∞], see
  /// [arb_zero_pm_inf]().
  ///
  ///     std::cout << arbxx::Arb::zero_pm_inf();
  ///     // -> [+/- inf]
  ///
  static Arb zero_pm_inf();

  /// Return an indeterminate, i.e., [NaN±∞] see [arb_indeterminate]().
  ///
  ///     std::cout << arbxx::Arb::indeterminate();
  ///     // -> nan
  ///
  static Arb indeterminate();

  /// Return the interval [-1, 1], i.e., the ball of radius one centered at
  /// zero, see [arb_zero_pm_one]().
  /// Note that this prints as `[+/- 1.01]` instead of `[+/- 1.00]`, see
  /// https://github.com/fredrik-johansson/arb/issues/391
  ///
  ///     std::cout << arbxx::Arb::zero_pm_one();
  ///     // -> [+/- 1.01]
  ///
  static Arb zero_pm_one();

  /// Return the unit interval [0, 1], i.e., the ball of radius 1/2 centered at
  /// 1/2, see [arb_unit_interval]().
  /// Note that that this does not print as `[0.5 +/- 0.5]`, see
  /// https://github.com/fredrik-johansson/arb/issues/391
  ///
  ///     std::cout << arbxx::Arb::unit_interval();
  ///     // -> [0.500000 +/- 0.501]
  ///
  static Arb unit_interval();

  /// Return a random element, see [arb_randtest]().
  ///
  ///     #include <flint/flintxx/frandxx.h>
  ///
  ///     flint::frandxx rand;
  ///     auto a = arbxx::Arb::randtest(rand, 64, 16);
  ///     auto b = arbxx::Arb::randtest(rand, 64, 16);
  ///     a.equal(b)
  ///     // -> false
  ///
  static Arb randtest(flint::frandxx&, prec precision, prec magbits);

  /// Return a random element, see [arb_randtest_exact]().
  ///
  ///     #include <flint/flintxx/frandxx.h>
  ///
  ///     flint::frandxx rand;
  ///     auto a = arbxx::Arb::randtest_exact(rand, 64, 16);
  ///     auto b = arbxx::Arb::randtest_exact(rand, 64, 16);
  ///     a.equal(b)
  ///     // -> false
  ///
  static Arb randtest_exact(flint::frandxx&, prec precision, prec magbits);

  /// Return whether elements have the same midpoint and radius.
  ///
  ///     arbxx::Arb a;
  ///     a.equal(a)
  ///     // -> true
  ///
  bool equal(const Arb&) const;

  /// Swap two elements efficiently, see [arb_swap]().
  /// Used by some STL containers.
  ///
  ///     arbxx::Arb a{1}, b;
  ///     std::cout << "a = " << a << ", b = " << b;
  ///     // -> a = 1.00000, b = 0
  ///
  ///     swap(a, b);
  ///     std::cout << "a = " << a << ", b = " << b;
  ///     // -> a = 0, b = 1.00000
  ///
  LIBARBXX_API friend void swap(Arb&, Arb&);

 private:
  /// TODO
  /// The underlying arb_t; use arb_t() to get a reference to it.
  ::arb_t t;
};

}  // namespace arbxx

#endif
