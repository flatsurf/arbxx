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

#include "external/catch2/single_include/catch2/catch.hpp"

#include "../arbxx/precision.hpp"

namespace arbxx::test {

TEST_CASE("Precision", "[precision]") {
  SECTION("No Precision is Normally Set") {
    REQUIRE_THROWS(Precision::current());
  }

  SECTION("Precision can be set for the Current Scope") {
    Precision prec{64};

    REQUIRE(Precision::current() == 64);

    {
      Precision prec32{32};

      REQUIRE(Precision::current() == 32);
    }

    REQUIRE(Precision::current() == 64);
  }

  SECTION("Precision is only set in the Current Thread") {
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
            REQUIRE_THROWS(Precision::current());
          }
          {
            
            std::lock_guard child_lock(mutex);

            {
              std::lock_guard catch_lock(catch_mutex);
              REQUIRE_THROWS(Precision::current());
            }
          }

          {
            std::lock_guard child_lock(mutex);

            Precision prec{32};

            {
              std::lock_guard catch_lock(catch_mutex);
              REQUIRE(Precision::current() == 32);
            }
          }

          {
            std::lock_guard catch_lock(catch_mutex);
            REQUIRE_THROWS(Precision::current());
          }
        } catch(...) {
          child_exception = std::current_exception();
        }
      });

      Precision prec{64};

      {
        std::lock_guard catch_lock(catch_mutex);
        REQUIRE(Precision::current() == 64);
      }
    }

    {
      std::lock_guard parent_lock(mutex);

      {
        std::lock_guard catch_lock(catch_mutex);
        REQUIRE_THROWS(Precision::current());
      }
    }

    child.join();

    if (child_exception)
      std::rethrow_exception(child_exception);
  }
}

}
