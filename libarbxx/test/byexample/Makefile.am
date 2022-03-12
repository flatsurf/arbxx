TESTS =                                                 \
	$(abs_srcdir)/../../arbxx/arf.hpp                     \
	$(abs_srcdir)/../../arbxx/precision.hpp               \
	$(abs_srcdir)/../../arbxx/rounding.hpp

# TODO
# $(abs_srcdir)/../../arbxx/arb.hpp                     \
# $(abs_srcdir)/../../arbxx/cereal.hpp                  \
# $(abs_srcdir)/../../arbxx/cppyy.hpp                   \
# $(abs_srcdir)/../../arbxx/forward.hpp

TEST_EXTENSIONS = .hpp .md
HPP_LOG_COMPILER = $(builddir)/manual-doctest.sh
MD_LOG_COMPILER = $(builddir)/manual-doctest.sh

EXTRA_DIST = $(TESTS)

AM_TESTS_ENVIRONMENT = . $(builddir)/test-env.sh;

@VALGRIND_CHECK_RULES@

BUILT_SOURCES = test-env.sh bin/python manual-doctest.sh
EXTRA_DIST += test-env.sh.in bin/python.in manual-doctest.sh.in
CLEANFILES = test-env.sh bin/python manual-doctest.sh

$(builddir)/test-env.sh: $(srcdir)/test-env.sh.in Makefile
	sed -e 's,[@]abs_builddir[@],$(abs_builddir),g' < $< > $@

$(builddir)/bin/python: $(srcdir)/bin/python.in Makefile
	mkdir -p $(builddir)/bin
	sed -e 's,[@]abs_srcdir[@],$(abs_srcdir),g' -e 's,[@]abs_builddir[@],$(abs_builddir),g' < $< > $@
	chmod +x $@

$(builddir)/manual-doctest.sh: $(srcdir)/manual-doctest.sh.in Makefile
	sed -e 's,[@]abs_srcdir[@],$(abs_srcdir),g' < $< > $@
	chmod +x $@
