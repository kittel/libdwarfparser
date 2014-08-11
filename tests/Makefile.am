AM_CPPFLAGS = -Wall -Wextra -Werror  -I $(top_builddir)/common
AM_CXXFLAGS = -fpic -I $(top_builddir) -rdynamic -std=c++1y
AM_CFLAGS = -fpic  -I $(top_builddir) 

bin_PROGRAMS = test \
			   dwarfsymbols

test_SOURCES = test.c

dwarfsymbols_SOURCES = dwarfsymbols.cpp
dwarfsymbols_CXXFLAGS = -I /home/kittel/projekte/code/libdwarf/ \
						$(AM_CXXFLAGS) \
						$(GLIB_CFLAGS) 
dwarfsymbols_LDFLAGS = -L../libdwarfparser/ \
					   -ldwarfparser -ldwarf -lelf \
					   -L/home/kittel/projekte/code/libdwarf/libdwarf
