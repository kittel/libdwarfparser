AM_CPPFLAGS = -Wall -Wextra -Werror  -I $(top_builddir)/common
AM_CXXFLAGS = -fpic -I $(top_builddir) -rdynamic -std=c++14
AM_CFLAGS = -fpic  -I $(top_builddir)

bin_PROGRAMS = dwarftestunit \
               tests \
               dwarfsymbols

dwarftestunit_SOURCES = dwarfTestUnit.c

tests_SOURCES = test.cpp \
                tests-dwarftests.cpp
tests_CXXFLAGS = $(AM_CXXFLAGS) \
                 $(GLIB_CFLAGS)

tests_LDFLAGS = -lvmiwrapper \
                -ldwarfparser -ldwarf -lelf \
                -lCppUTest -lCppUTestExt

dwarfsymbols_SOURCES = dwarfsymbols.cpp
dwarfsymbols_CXXFLAGS = $(AM_CXXFLAGS) \
                        $(GLIB_CFLAGS)
dwarfsymbols_LDFLAGS = -lvmiwrapper \
                       -ldwarfparser -ldwarf -lelf \
                       -lCppUTest -lCppUTestExt
