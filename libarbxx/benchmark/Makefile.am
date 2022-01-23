noinst_PROGRAMS = benchmark

benchmark_SOURCES = main.cc arb.benchmark.cc

AM_CPPFLAGS = -I $(srcdir)/.. -I $(builddir)/..
AM_LDFLAGS = $(builddir)/../src/libarbxx.la
AM_LDFLAGS += -lgmp -larb
# Google Benchmark and its dependencies
AM_LDFLAGS += -lbenchmark -lrt -lpthread

