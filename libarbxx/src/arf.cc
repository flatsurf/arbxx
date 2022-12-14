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

#include "../arbxx/arf.hpp"
#include "../arbxx/precision.hpp"
#include "../arbxx/rounding.hpp"

#include <arf.h>

#include <ostream>
#include <stdexcept>

#include "util/integer.ipp"

namespace {

std::pair<mpz_class, mpz_class> mantissa_exponent(const arf_t arf) {
  fmpz_t mantissa;
  fmpz_t exponent;
  fmpz_init(mantissa);
  fmpz_init(exponent);

  arf_get_fmpz_2exp(mantissa, exponent, arf);

  mpz_class m, e;
  fmpz_get_mpz(m.get_mpz_t(), mantissa);
  fmpz_get_mpz(e.get_mpz_t(), exponent);

  fmpz_clear(mantissa);
  fmpz_clear(exponent);

  return std::make_pair(m, e);
}

}  // namespace

namespace arbxx {

Arf::Arf(const std::string& mantissa, int base, long exponent) : Arf() {
  fmpz_t m, e;
  fmpz_init(m);
  fmpz_init(e);

  const bool error = fmpz_set_str(m, mantissa.c_str(), base) == -1;
  if (!error) {
    fmpz_set_si(e, exponent);
    arf_set_fmpz_2exp(t, m, e);
  }

  fmpz_clear(e);
  fmpz_clear(m);

  if (error)
    throw std::invalid_argument("cannot parse mantissa");
}

Arf::Arf(const mpz_class& mantissa, long exponent) : Arf() {
  fmpz_t m, e;
  fmpz_init(m);
  fmpz_init(e);

  fmpz_set_mpz(m, mantissa.get_mpz_t());
  fmpz_set_si(e, exponent);
  arf_set_fmpz_2exp(t, m, e);

  fmpz_clear(e);
  fmpz_clear(m);
}

Arf::Arf(long long value) : Arf() {
  *this = value;
}

Arf::Arf(unsigned long long value) : Arf() {
  *this = value;
}

Arf::Arf(double value) : Arf() { arf_set_d(t, value); }

Arf::operator double() const { return arf_get_d(t, ARF_RND_NEAR); }

Arf& Arf::operator=(const Arf& rhs) noexcept {
  arf_set(t, rhs.t);
  return *this;
}

Arf& Arf::operator=(Arf&& rhs) noexcept {
  swap(*this, rhs);
  return *this;
}

Arf& Arf::operator=(short rhs) {
  return *this = to_supported_integer(rhs);
}

Arf& Arf::operator=(unsigned short rhs) {
  return *this = to_supported_integer(rhs);
}

Arf& Arf::operator=(int rhs) {
  return *this = to_supported_integer(rhs);
}

Arf& Arf::operator=(unsigned int rhs) {
  return *this = to_supported_integer(rhs);
}

Arf& Arf::operator=(long rhs) {
  arf_set_si(t, rhs);
  return *this;
}

Arf& Arf::operator=(unsigned long rhs) {
  arf_set_ui(t, rhs);
  return *this;
}

Arf& Arf::operator=(long long rhs) {
  return *this = to_supported_integer(rhs);
}

Arf& Arf::operator=(unsigned long long rhs) {
  return *this = to_supported_integer(rhs);
}

Arf& Arf::operator=(const mpz_class& rhs) {
  fmpz_t x;
  fmpz_init_set_readonly(x, rhs.get_mpz_t());
  arf_set_fmpz(arf_t(), x);
  fmpz_clear_readonly(x);

  return *this;
}

Arf& Arf::operator=(double rhs) {
  arf_set_d(arf_t(), rhs);
  return *this;
}

Arf& Arf::operator<<=(long rhs) {
  arf_mul_2exp_si(t, t, rhs);
  return *this;
}

Arf& Arf::operator>>=(long rhs) { return this->operator<<=(-rhs); }

bool operator<(const Arf& lhs, const Arf& rhs) { return arf_cmp(lhs.t, rhs.t) < 0; }

bool operator==(const Arf& lhs, const Arf& rhs) { return arf_equal(lhs.t, rhs.t); }

bool operator<(const Arf& lhs, short rhs) { return lhs < to_supported_integer(rhs); }

bool operator>(const Arf& lhs, short rhs) { return lhs > to_supported_integer(rhs); }

bool operator==(const Arf& lhs, short rhs) { return lhs == to_supported_integer(rhs); }

bool operator<(const Arf& lhs, unsigned short rhs) { return lhs < to_supported_integer(rhs); }

bool operator>(const Arf& lhs, unsigned short rhs) { return lhs > to_supported_integer(rhs); }

bool operator==(const Arf& lhs, unsigned short rhs) { return lhs == to_supported_integer(rhs); }

bool operator<(const Arf& lhs, int rhs) { return lhs < to_supported_integer(rhs); }

bool operator>(const Arf& lhs, int rhs) { return lhs > to_supported_integer(rhs); }

bool operator==(const Arf& lhs, int rhs) { return lhs == to_supported_integer(rhs); }

bool operator<(const Arf& lhs, unsigned int rhs) { return lhs < to_supported_integer(rhs); }

bool operator>(const Arf& lhs, unsigned int rhs) { return lhs > to_supported_integer(rhs); }

bool operator==(const Arf& lhs, unsigned int rhs) { return lhs == to_supported_integer(rhs); }

bool operator<(const Arf& lhs, long rhs) { return arf_cmp_si(lhs.t, rhs) < 0; }

bool operator>(const Arf& lhs, long rhs) { return arf_cmp_si(lhs.t, rhs) > 0; }

bool operator==(const Arf& lhs, long rhs) { return arf_equal_si(lhs.t, rhs); }

bool operator<(const Arf& lhs, unsigned long rhs) { return arf_cmp_ui(lhs.t, rhs) < 0; }

bool operator>(const Arf& lhs, unsigned long rhs) { return arf_cmp_ui(lhs.t, rhs) > 0; }

bool operator==(const Arf& lhs, unsigned long rhs) { return arf_cmp_ui(lhs.t, rhs) == 0; }

bool operator<(const Arf& lhs, long long rhs) { return lhs < to_supported_integer(rhs); }

bool operator>(const Arf& lhs, long long rhs) { return lhs > to_supported_integer(rhs); }

bool operator==(const Arf& lhs, long long rhs) { return lhs == to_supported_integer(rhs); }

bool operator<(const Arf& lhs, unsigned long long rhs) { return lhs < to_supported_integer(rhs); }

bool operator>(const Arf& lhs, unsigned long long rhs) { return lhs > to_supported_integer(rhs); }

bool operator==(const Arf& lhs, unsigned long long rhs) { return lhs == to_supported_integer(rhs); }

bool operator<(const Arf& lhs, const mpz_class& rhs) {
  Arf rhs_(rhs);
  return lhs < rhs_;
}

bool operator>(const Arf& lhs, const mpz_class& rhs) {
  Arf rhs_(rhs);
  return lhs > rhs_;
}

bool operator==(const Arf& lhs, const mpz_class& rhs) {
  Arf rhs_(rhs);
  return lhs == rhs_;
}

Arf Arf::operator-() const {
  Arf ret;
  arf_neg(ret.t, t);
  return ret;
}

Arf& operator+=(Arf& lhs, const Arf& rhs) {
  arf_add(lhs, lhs, rhs, Precision::current(), Rounding::current());
  return lhs;
}

Arf& operator-=(Arf& lhs, const Arf& rhs) {
  arf_sub(lhs, lhs, rhs, Precision::current(), Rounding::current());
  return lhs;
}

Arf& operator*=(Arf& lhs, const Arf& rhs) {
  arf_mul(lhs, lhs, rhs, Precision::current(), Rounding::current());
  return lhs;
}

Arf& operator/=(Arf& lhs, const Arf& rhs) {
  arf_div(lhs, lhs, rhs, Precision::current(), Rounding::current());
  return lhs;
}

Arf Arf::abs() const {
  Arf ret;
  arf_abs(ret.t, t);
  return ret;
}

mpz_class Arf::floor() const {
  Arf floor;
  arf_floor(floor.t, t);
  fmpz_t fmpz_floor;
  fmpz_init(fmpz_floor);
  arf_get_fmpz(fmpz_floor, floor.t, ARF_RND_NEAR);
  mpz_class ret;
  fmpz_get_mpz(ret.get_mpz_t(), fmpz_floor);
  fmpz_clear(fmpz_floor);
  return ret;
}

mpz_class Arf::ceil() const {
  Arf ceil;
  arf_ceil(ceil.t, t);
  fmpz_t fmpz_ceil;
  fmpz_init(fmpz_ceil);
  arf_get_fmpz(fmpz_ceil, ceil.t, ARF_RND_NEAR);
  mpz_class ret;
  fmpz_get_mpz(ret.get_mpz_t(), fmpz_ceil);
  fmpz_clear(fmpz_ceil);
  return ret;
}

mpz_class Arf::mantissa() const { return mantissa_exponent(t).first; }

mpz_class Arf::exponent() const { return mantissa_exponent(t).second; }

long Arf::logb() const {
  Arf _;
  fmpz_t e;
  fmpz_init(e);
  arf_frexp(_.t, e, t);
  long ret = fmpz_get_si(e) - 1;
  fmpz_clear(e);
  return ret;
}

Arf Arf::randtest(flint::frandxx& state, prec precision, prec magbits) {
  Arf ret;
  arf_randtest(ret.arf_t(), state._data(), precision, magbits);
  return ret;
}

std::ostream& operator<<(std::ostream& os, const Arf& self) {
  if (arf_is_zero(self.t)) {
    return os << 0;
  } else if (arf_is_pos_inf(self.t)) {
    return os << "+∞";
  } else if (arf_is_neg_inf(self.t)) {
    return os << "-∞";
  } else if (arf_is_nan(self.t)) {
    return os << "NaN";
  } else if (self.exponent() >= 0) {
    return os << static_cast<double>(self);
  }

  os << static_cast<double>(self) << "=" << self.mantissa() << "p" << self.exponent();

  return os;
}

}  // namespace arbxx

namespace std {

size_t hash<arbxx::Arf>::operator()(const arbxx::Arf& self) const {
  return hash<double>()(static_cast<double>(self));
}

}  // namespace std
