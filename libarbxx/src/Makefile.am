lib_LTLIBRARIES = libarbxx.la

libarbxx_la_SOURCES =               \
    arb.cc                              \
    arf.cc

libarbxx_la_LDFLAGS = -version-info $(libarbxx_version_info)

if HAVE_VERSION_SCRIPT
libarbxx_la_LDFLAGS = -Wl,--version-script=$(srcdir)/libarbxx.map

EXTRA_libarbxx_la_DEPENDENCIES = $(srcdir)/libarbxx.map
endif

nobase_pkginclude_HEADERS =                                  \
    ../arbxx/arb.hpp                                    \
    ../arbxx/arf.hpp                                    \
    ../arbxx/cereal.hpp                                 \
    ../arbxx/cppyy.hpp

noinst_HEADERS =                                               \
    external/gmpxxll/gmpxxll/mpz_class.hpp                     \
    util/assert.ipp                                            \
    util/integer.ipp

$(builddir)/../arbxx/local.hpp: $(srcdir)/../arbxx/local.hpp.in Makefile
	mkdir -p $(builddir)/libarbxx
	sed -e 's,[@]libdir[@],$(libdir),g' < $< > $@

# Since we generate local.hpp, we need to search $(builddir) during out-of-tree builds aka VPATH builds
VPATH = $(srcdir) $(builddir)
# Generate local.hpp before performing any compilation
BUILT_SOURCES = ../arbxx/local.hpp
# Distribute local.hpp.in as part of the tarball, but not local.hpp
EXTRA_DIST = ../arbxx/local.hpp.in
# We vendor the header-only library Cereal (serialization with C++) which we
# use in some of our source files.
# Cereal is basically not packaged in any distribution since it is a
# header-only library that people always vendor into their source tree.
EXTRA_DIST += external/cereal/include
generatedincludedir = $(includedir)/arbxx
nodist_generatedinclude_HEADERS = ../arbxx/local.hpp

# We are building the libarbxx library so visibility attributes should be
# set, see https://gcc.gnu.org/wiki/Visibility
AM_CPPFLAGS = -DBUILD_LIBARBXX

CLEANFILES = ../arbxx/local.hpp
