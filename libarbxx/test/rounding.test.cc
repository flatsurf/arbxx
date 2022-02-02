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

#include <exception>
#include <thread>
#include <condition_variable>
#include <mutex>

#include <arf.h>

#include "external/catch2/single_include/catch2/catch.hpp"

#include "../arbxx/rounding.hpp"

namespace arbxx::test {

TEST_CASE("Rounding", "[rounding]") {
  SECTION("No Rounding Direction is Normally Set") {
    REQUIRE_THROWS(Rounding::current());
  }

  SECTION("Only Supported Rounding Directions can be Set") {
    REQUIRE_THROWS(Rounding{1337});

    const auto ROUNDING = GENERATE(ARF_RND_DOWN, ARF_RND_UP, ARF_RND_FLOOR, ARF_RND_CEIL, ARF_RND_NEAR);

    Rounding rounding{ROUNDING};

    REQUIRE(Rounding::current() == ROUNDING);
  }

  SECTION("A Rounding Direction can be set for the Current Scope") {
    Rounding rounding{ARF_RND_DOWN};

    REQUIRE(Rounding::current() == ARF_RND_DOWN);

    {
      Rounding rounding2{ARF_RND_UP};

      REQUIRE(Rounding::current() == ARF_RND_UP);
    }

    REQUIRE(Rounding::current() == ARF_RND_DOWN);
  }

  SECTION("Rounding Direction is only set in the Current Thread") {
    std::mutex mutex;

    // Lock for calls to Catch2 which is not thread-safe.
    std::mutex catch_mutex;

    std::exception_ptr child_exception;

    std::thread child;

    {
      std::lock_guard parent_lock(mutex);

      child = std::thread([&]() {
        try {
          {
            std::lock_guard catch_lock(catch_mutex);
            REQUIRE_THROWS(Rounding::current());
          }
          {
            
            std::lock_guard child_lock(mutex);

            {
              std::lock_guard catch_lock(catch_mutex);
              REQUIRE_THROWS(Rounding::current());
            }
          }

          {
            std::lock_guard child_lock(mutex);

            Rounding rounding{ARF_RND_DOWN};

            {
              std::lock_guard catch_lock(catch_mutex);
              REQUIRE(Rounding::current() == ARF_RND_DOWN);
            }
          }

          {
            std::lock_guard catch_lock(catch_mutex);
            REQUIRE_THROWS(Rounding::current());
          }
        } catch(...) {
          child_exception = std::current_exception();
        }
      });

      Rounding rounding{ARF_RND_UP};

      {
        std::lock_guard catch_lock(catch_mutex);
        REQUIRE(Rounding::current() == ARF_RND_UP);
      }
    }

    {
      std::lock_guard parent_lock(mutex);

      {
        std::lock_guard catch_lock(catch_mutex);
        REQUIRE_THROWS(Rounding::current());
      }
    }

    child.join();

    if (child_exception)
      std::rethrow_exception(child_exception);
  }
}

}

