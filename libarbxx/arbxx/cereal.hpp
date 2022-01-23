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

#ifndef LIBARBXX_CEREAL_HPP
#define LIBARBXX_CEREAL_HPP

#include <cereal/cereal.hpp>

#include "arb.hpp"
#include "arf.hpp"

namespace arbxx {

template <typename Archive>
void save(Archive& archive, const Arb& self) {
  char* serialized = arb_dump_str(self.arb_t());
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

  if (arb_load_str(self.arb_t(), data.c_str())) {
    throw std::logic_error("malformed archive, failed to parse Arb");
  }
}

template <typename Archive>
void save(Archive& archive, const Arf& self) {
  char* serialized = arf_dump_str(self.arf_t());
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

  if (arf_load_str(self.arf_t(), data.c_str())) {
    throw std::logic_error("malformed archive, failed to parse Arf");
  }
}

}  // namespace arbxx

#endif
