/**********************************************************************
 *  This file is part of arbxx.
 *
 *        Copyright (C) 2020-2022 Julian Rüth
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

#ifndef LIBARBXX_UTIL_ASSERT_IPP
#define LIBARBXX_UTIL_ASSERT_IPP

#include <boost/algorithm/string.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <cstdlib>
#include <iostream>
#include <sstream>

namespace arbxx {
namespace {

// A throw statement that can be used in noexcept marked blocks without
// triggering compiler warnings.
template <typename E>
void throw_for_assert(const E& e) { throw e; }

// Return whether an environment variable should be considered as set.
bool isSet(const char* env) {
  const auto* ptr = std::getenv(env);
  if (ptr == nullptr) return false;

  std::string value = ptr;
  boost::trim(value);

  if (value == "0") return false;
  if (boost::iequals(value, "no")) return false;
  if (boost::iequals(value, "false")) return false;

  return true;
}

// Return whether all LIBARBXX_CHECK_ and LIBARBXX_ASSERT_ macros have
// been disabled at runtime through the LIBARBXX_NOCHECK environment
// variable.
bool nocheck() {
  static bool value = isSet("LIBARBXX_NOCHECK");
  return value;
}

// Return whether all LIBARBXX_ASSERT_ macros have been disabled at runtime
// through the LIBARBXX_NOASSERT environment variable.
bool noassert() {
  if (nocheck()) return true;

  static bool value = isSet("LIBARBXX_NOASSERT");
  return value;
}

}  // namespace
}  // namespace arbxx

#define LIBARBXX_ASSERT_(CONDITION, EXCEPTION, MESSAGE)                    \
  while (BOOST_UNLIKELY(static_cast<bool>(not(CONDITION)))) {              \
    std::stringstream user_message, assertion_message;                     \
    user_message << MESSAGE;                                               \
    assertion_message << (#CONDITION " does not hold");                    \
    if (user_message.str().size())                                         \
      assertion_message << ": " << user_message.str();                     \
    else                                                                   \
      assertion_message << " ";                                            \
    assertion_message << " in " __FILE__ ":" BOOST_PP_STRINGIZE(__LINE__); \
    /* show messages in noexcept blocks */                                 \
    std::cerr << assertion_message.str() << std::endl;                     \
    ::arbxx::throw_for_assert(EXCEPTION(assertion_message.str().c_str())); \
  }

// Run a (cheap) check that a (user provided) argument is valid.
// If the check should be disabled when NDEBUG is defined, e.g., because it
// occurs in a hotspot, use LIBARBXX_ASSERT_ARGUMENT instead.
#define LIBARBXX_CHECK_ARGUMENT_(CONDITION) LIBARBXX_ASSERT_(::arbxx::nocheck() || (CONDITION), std::invalid_argument, "")
#define LIBARBXX_CHECK_ARGUMENT(CONDITION, MESSAGE) LIBARBXX_ASSERT_(::arbxx::nocheck() || (CONDITION), std::invalid_argument, MESSAGE)

#ifdef NDEBUG

#define LIBARBXX_ASSERT_CONDITION(CONDITION) (true || ::arbxx::noassert() || (CONDITION))

#else

#define LIBARBXX_ASSERT_CONDITION(CONDITION) (::arbxx::noassert() || (CONDITION))

#endif

#define LIBARBXX_ASSERT_ARGUMENT_(CONDITION) CHECK_ARGUMENT_(LIBARBXX_ASSERT_CONDITION(CONDITION))
#define LIBARBXX_ASSERT_ARGUMENT(CONDITION, MESSAGE) CHECK_ARGUMENT(LIBARBXX_ASSERT_CONDITION(CONDITION), MESSAGE)
#define LIBARBXX_ASSERT(CONDITION, MESSAGE) LIBARBXX_ASSERT_(LIBARBXX_ASSERT_CONDITION(CONDITION), std::logic_error, MESSAGE)

#define LIBARBXX_UNREACHABLE(MESSAGE)                  \
  {                                                    \
    LIBARBXX_ASSERT_(false, std::logic_error, MESSAGE) \
    __builtin_unreachable();                           \
  }

#endif
