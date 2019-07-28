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

#ifndef LIBEXACTREAL_CEREAL_HPP
#define LIBEXACTREAL_CEREAL_HPP

#include <e-antic/renfxx.h>
#include <boost/lexical_cast.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

#include "arb.hpp"
#include "arf.hpp"
#include "element.hpp"
#include "integer_ring.hpp"
#include "module.hpp"
#include "number_field.hpp"
#include "rational_field.hpp"
#include "real_number.hpp"

namespace exactreal {

// A trivial wrapper for a T so we do not pollute the global namespace with our
// serialization function for T.
template <typename T>
struct CerealWrap {
  CerealWrap() : inner() {}
  CerealWrap(T&& inner) : inner(std::move(inner)) {}
  CerealWrap(const T& inner) : inner(inner) {}

  T inner;

  T* operator->() { return &inner; }
  const T* operator->() const { return &inner; }

  T& operator*() { return inner; }
  const T& operator*() const { return inner; }
};

// This should go int arb: https://github.com/fredrik-johansson/arb/issues/283
namespace {
const char* arf_serialize_zero = "0 0";
const char* arf_serialize_pos_inf = "0 -1";
const char* arf_serialize_neg_inf = "0 -2";
const char* arf_serialize_nan = "0 -3";

char* arf_serialize(const arf_t x) {
  if (arf_is_special(x)) {
    const char* ret;
    if (arf_is_zero(x)) {
      ret = arf_serialize_zero;
    } else if (arf_is_pos_inf(x)) {
      ret = arf_serialize_pos_inf;
    } else if (arf_is_neg_inf(x)) {
      ret = arf_serialize_neg_inf;
    } else if (arf_is_nan(x)) {
      ret = arf_serialize_nan;
    } else {
      // Impossible to happen; all the special values have been treated above.
      assert(false);
    }
    char* res = (char*)flint_malloc(strlen(ret) + 1);
    strcpy(res, ret);
    return res;
  } else {
    fmpz_t mantissa, exponent;

    fmpz_init(mantissa);
    fmpz_init(exponent);

    arf_get_fmpz_2exp(mantissa, exponent, x);

    char* res = (char*)flint_malloc(fmpz_sizeinbase(mantissa, 16) + 1 + fmpz_sizeinbase(exponent, 16));

    fmpz_get_str(res, 16, mantissa);
    strcat(res, " ");
    fmpz_get_str(res + strlen(res), 16, exponent);

    fmpz_clear(mantissa);
    fmpz_clear(exponent);

    return res;
  }
}

int arf_deserialize(arf_t x, const char* data) {
  if (strcmp(data, arf_serialize_zero) == 0) {
    arf_zero(x);
  } else if (strcmp(data, arf_serialize_neg_inf) == 0) {
    arf_neg_inf(x);
  } else if (strcmp(data, arf_serialize_pos_inf) == 0) {
    arf_pos_inf(x);
  } else if (strcmp(data, arf_serialize_nan) == 0) {
    arf_nan(x);
  } else {
    fmpz_t mantissa, exponent;

    fmpz_init(mantissa);
    fmpz_init(exponent);

    const char* e_str = strchr(data, ' ');

    if (e_str == NULL) {
      return 1;
    }

    char* m_str = (char*)flint_malloc(e_str - data + 1);
    strncpy(m_str, data, e_str - data);
    m_str[e_str - data] = '\0';

    int err = fmpz_set_str(mantissa, m_str, 16);

    flint_free(m_str);

    if (err) {
      fmpz_clear(exponent);
      fmpz_clear(mantissa);
      return err;
    }

    err = fmpz_set_str(exponent, e_str + 1, 16);

    if (err) {
      fmpz_clear(exponent);
      fmpz_clear(mantissa);
      return err;
    }

    arf_set_fmpz_2exp(x, mantissa, exponent);

    fmpz_clear(exponent);
    fmpz_clear(mantissa);
  }

  return 0;
}

char* mag_serialize(const mag_t x) {
  arf_t y;
  arf_init(y);
  arf_set_mag(y, x);
  char* res = arf_serialize(y);
  arf_clear(y);
  return res;
}

int mag_deserialize(mag_t x, const char* data) {
  arf_t y;
  arf_init(y);

  int err = arf_deserialize(y, data);
  if (err) {
    arf_clear(y);
    return err;
  }

  fmpz_t mantissa, exponent;
  fmpz_init(mantissa);
  fmpz_init(exponent);

  arf_get_fmpz_2exp(mantissa, exponent, y);

  assert(fmpz_cmp_ui(mantissa, 1 << MAG_BITS) < 0);

  mag_set_ui(x, fmpz_get_ui(mantissa));

  mag_mul_2exp_fmpz(x, x, exponent);

  fmpz_clear(exponent);
  fmpz_clear(mantissa);
  arf_clear(y);

  return err;
}

char* arb_serialize(const arb_t x) {
  char* mid = arf_serialize(arb_midref(x));
  char* mag = mag_serialize(arb_radref(x));

  char* res = (char*)flint_malloc(strlen(mid) + 1 + strlen(mag) + 1);
  strcpy(res, mid);
  strcat(res, " ");
  strcat(res, mag);

  flint_free(mid);
  flint_free(mag);

  return res;
}

int arb_deserialize(arb_t x, const char* data) {
  const char* split = strchr(data, ' ');
  if (split == NULL) {
    return 1;
  }
  split = strchr(split + 1, ' ');
  if (split == NULL) {
    return 1;
  }

  size_t midlen = (size_t)(split - data);
  char* mid = (char*)flint_malloc(midlen + 1);
  strncpy(mid, data, midlen);
  mid[midlen] = '\0';

  size_t maglen = strlen(data) - midlen - 1;
  char* mag = (char*)flint_malloc(maglen + 1);
  strncpy(mag, split + 1, maglen);
  mag[maglen] = '\0';

  int err = arf_deserialize(arb_midref(x), mid);

  if (!err) {
    err = mag_deserialize(arb_radref(x), mag);
  }

  flint_free(mid);
  flint_free(mag);
  return err;
}
}  // namespace

template <typename Archive>
void save(Archive& archive, const Arb& self) {
  char* serialized = arb_serialize(self.arb_t());
  archive(
      cereal::make_nvp("data", std::string(serialized)),
      cereal::make_nvp("approximation", static_cast<double>(self)));
  flint_free(serialized);
}

template <typename Archive>
void load(Archive& archive, Arb& self) {
  std::string data;
  double ignored;
  archive(data, ignored);

  if (arb_deserialize(self.arb_t(), data.c_str())) {
    throw std::logic_error("malformed archive, failed to parse Arb");
  }
}

template <typename Archive>
void save(Archive& archive, const Arf& self) {
  char* serialized = arf_serialize(self.arf_t());
  archive(
      cereal::make_nvp("data", std::string(serialized)),
      cereal::make_nvp("approximation", static_cast<double>(self)));
  flint_free(serialized);
}

template <typename Archive>
void load(Archive& archive, Arf& self) {
  std::string data;
  double ignored;
  archive(data, ignored);

  if (arf_deserialize(self.arf_t(), data.c_str())) {
    throw std::logic_error("malformed archive, failed to parse Arf");
  }
}

template <typename Archive, typename Ring>
void save(Archive& archive, const Element<Ring>& self) {
  archive(cereal::make_nvp("parent", self.module()));

  std::vector<CerealWrap<typename Ring::ElementClass>> coefficients;
  for (int i = 0; i < self.module()->rank(); i++) {
    coefficients.push_back(self[i]);
  }
  archive(cereal::make_nvp("coefficients", coefficients));
}

template <typename Archive, typename Ring>
void load(Archive& archive, Element<Ring>& self) {
  std::shared_ptr<const Module<Ring>> module;
  archive(cereal::make_nvp("parent", module));

  std::vector<CerealWrap<typename Ring::ElementClass>> coefficients;
  archive(cereal::make_nvp("coefficients", coefficients));

  std::vector<typename Ring::ElementClass> coeffs;
  for (auto& c : coefficients) {
    coeffs.push_back(*c);
  }

  self = Element<Ring>(module, coeffs);
}

template <typename Archive>
void serialize(Archive&, IntegerRing&) {
  // There's nothing to do for ZZ.
}

template <typename Archive>
void serialize(Archive&, RationalField&) {
  // There's nothing to do for QQ.
}

template <typename Archive>
void serialize(Archive& archive, NumberField& self) {
  archive(cereal::make_nvp("field", self.parameters));
}

struct RealNumberCereal {
  using SupportedOutput = cereal::JSONOutputArchive;
  using SupportedInput = cereal::JSONInputArchive;

  static void save(cereal::JSONOutputArchive& archive, const std::shared_ptr<const RealNumber>& self);
  static void load(cereal::JSONInputArchive& archive, std::shared_ptr<const RealNumber>& self);
};

template <typename T, typename Cerealizer>
struct ForwardingCereal {
  template <typename Archive>
  static void save(Archive& archive, const T& self) {
    using Native = typename Cerealizer::SupportedOutput;

    if constexpr (std::is_same_v<Archive, Native>) {
      Cerealizer::save(archive, self);
    } else {
      std::stringstream s;
      save(Native(s), self);
      archive(s.str());
    }
  }

  template <typename Archive>
  static void load(Archive& archive, T& self) {
    using Native = typename Cerealizer::SupportedInput;

    if constexpr (std::is_same_v<Archive, Native>) {
      Cerealizer::load(archive, self);
    } else {
      std::string s;
      archive(s);
      load(Native(std::stringstream(s)), self);
    }
  }
};

template <typename Archive>
void save(Archive& archive, const CerealWrap<mpq_class>& self) {
  archive(cereal::make_nvp("numerator", CerealWrap{self->get_num()}),
          cereal::make_nvp("denominator", CerealWrap{self->get_den()}));
}

template <typename Archive>
void load(Archive& archive, CerealWrap<mpq_class>& self) {
  CerealWrap<mpz_class> num, den;
  archive(num, den);
  self = CerealWrap{mpq_class(*num, *den)};
}

template <typename Archive>
void save(Archive& archive, const CerealWrap<mpz_class>& self) {
  archive(cereal::make_nvp("value", boost::lexical_cast<std::string>(*self)));
}

template <typename Archive>
void load(Archive& archive, CerealWrap<mpz_class>& self) {
  std::string value;
  archive(value);
  self = CerealWrap{mpz_class(value)};
}

template <typename Archive, typename T>
void save(Archive& archive, const CerealWrap<T>& self) {
  archive(*self);
}

template <typename Archive, typename T>
void load(Archive& archive, CerealWrap<T>& self) {
  archive(*self);
}
}  // namespace exactreal

namespace cereal {
template <typename Archive>
void save(Archive& archive, const std::shared_ptr<const exactreal::RealNumber>& self) {
  // cereal has a fairly opinionated way of handling smart pointers to
  // polymorphic types. This is not compatibly with our hiding logic of the
  // implementation, so we hook into cereal's registerSharedPointer to get our
  // real numbers properly deduplicated but handle the actual serialization &
  // deserialization ourselves through our factory functions.
  uint32_t id = archive.registerSharedPointer(self.get());

  archive(cereal::make_nvp("shared", id));
  if (id & cereal::detail::msb_32bit) {
    exactreal::ForwardingCereal<std::shared_ptr<const exactreal::RealNumber>, exactreal::RealNumberCereal>::save(archive, self);
  }
}

template <typename Archive>
void load(Archive& archive, std::shared_ptr<const exactreal::RealNumber>& self) {
  uint32_t id;
  archive(cereal::make_nvp("shared", id));

  if (id & cereal::detail::msb_32bit) {
    exactreal::ForwardingCereal<std::shared_ptr<const exactreal::RealNumber>, exactreal::RealNumberCereal>::load(archive, self);
    archive.registerSharedPointer(id, std::static_pointer_cast<void>(std::const_pointer_cast<exactreal::RealNumber>(self)));
  } else {
    self = std::static_pointer_cast<const exactreal::RealNumber>(archive.getSharedPointer(id));
  }
}

template <typename Archive, typename Ring>
void save(Archive& archive, const std::shared_ptr<const exactreal::Module<Ring>>& self) {
  uint32_t id = archive.registerSharedPointer(self.get());

  archive(cereal::make_nvp("shared", id));
  if (id & cereal::detail::msb_32bit) {
    archive(cereal::make_nvp("basis", self->basis()));
    archive(cereal::make_nvp("ring", self->ring()));
  }
}

template <typename Archive, typename Ring>
void load(Archive& archive, std::shared_ptr<const exactreal::Module<Ring>>& self) {
  // cereal insists on creating a new instance of T when deserializing a
  // shared_ptr<T>. However this is not compatible with our uniqueness of
  // modules (if the module already exists, we do not create a new one but just
  // return a shared_ptr to the existing one.) Therefore, we hook into cereal's
  // deduplication logic for a compact output format but do not rely on its
  // deserialization for Module<Ring>.
  uint32_t id;
  archive(cereal::make_nvp("shared", id));

  if (id & cereal::detail::msb_32bit) {
    typename exactreal::Module<Ring>::Basis basis;
    archive(cereal::make_nvp("basis", basis));
    Ring ring;
    archive(cereal::make_nvp("ring", ring));
    self = exactreal::Module<Ring>::make(basis, ring);
  } else {
    self = std::static_pointer_cast<exactreal::Module<Ring>>(archive.getSharedPointer(id));
  }
}
}  // namespace cereal

#endif
