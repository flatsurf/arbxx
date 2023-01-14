if HAVE_BYEXAMPLE
    MAYBE_BYEXAMPLE = byexample
endif

SUBDIRS = $(MAYBE_BYEXAMPLE)

check_PROGRAMS = main

TESTS = libarbxx.test.sh

main_SOURCES = arb.test.cc arf.test.cc cereal.test.cc cppyy.test.cc precision.test.cc rounding.test.cc main.cc

noinst_HEADERS =                                    \
  arf.hpp                                           \
	arb.hpp

# We vendor the header-only library Cereal (serialization with C++ to be able
# to run the tests even when cereal is not installed.
# Cereal is basically not packaged in any distribution since it is a
# header-only library that people always vendor into their source tree.
EXTRA_DIST = external/cereal/include
# We vendor the header-only test library catch2
EXTRA_DIST += external/catch2/single_include/catch2/catch.hpp

EXTRA_DIST += libflatsurf.test.sh

@VALGRIND_CHECK_RULES@

AM_CPPFLAGS = -I $(srcdir)/../src/ -I $(builddir)/../src/ -isystem $(srcdir)/external/cereal/include
AM_LDFLAGS = $(builddir)/../src/libarbxx.la
# arb.hpp & arf.hpp use gmpxx; it appears that we need to link gmp in explicitly as well to make gmpxx happy
AM_LDFLAGS += -lgmpxx -lgmp
# arb.hpp & arf.hpp use flint
AM_LDFLAGS += -lflint
# tests use threading
AM_LDFLAGS += -lpthread
