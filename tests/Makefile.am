AM_CPPFLAGS = -Wall -Wextra -Werror  -I $(top_builddir)/common
AM_CXXFLAGS = -fpic -I $(top_builddir) -rdynamic -std=c++1y
AM_CFLAGS = -fpic  -I $(top_builddir) 

bin_PROGRAMS = dwarftestunit \
			   tests \
			   dwarfsymbols

dwarftestunit_SOURCES = dwarfTestUnit.c

tests_SOURCES = test.cpp \
				tests-dwarftests.cpp
tests_CXXFLAGS = -I /home/kittel/projekte/code/libdwarf/ \
				 $(AM_CXXFLAGS) \
				 $(GLIB_CFLAGS) 

tests_LDFLAGS = \
				-L../libdwarfparser/ \
				-L../libvmiwrapper/ \
				-L/home/kittel/projekte/code/libdwarf/libdwarf \
				-lvmiwrapper \
				-ldwarfparser -ldwarf -lelf \
				-lCppUTest -lCppUTestExt

dwarfsymbols_SOURCES = dwarfsymbols.cpp
dwarfsymbols_CXXFLAGS = -I /home/kittel/projekte/code/libdwarf/ \
						$(AM_CXXFLAGS) \
						$(GLIB_CFLAGS) 
dwarfsymbols_LDFLAGS = \
					   -L../libdwarfparser/ \
					   -L../libvmiwrapper/ \
					   -lvmiwrapper \
					   -ldwarfparser -ldwarf -lelf \
					   -L/home/kittel/projekte/code/libdwarf/libdwarf
