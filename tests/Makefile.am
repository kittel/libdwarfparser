AM_CPPFLAGS = -Wall -Wextra -Werror 
AM_CXXFLAGS = -fpic -I $(top_builddir) -rdynamic -std=c++11
AM_CFLAGS = -fpic  -I $(top_builddir) 

bin_PROGRAMS = test \
			   dwarfsymbols

test_SOURCES = test.c

dwarfsymbols_SOURCES = dwarfsymbols.cpp
dwarfsymbols_CXXFLAGS = -I /home/kittel/projekte/libvmi/libdwarf/libdwarf/ $(AM_CXXFLAGS)
dwarfsymbols_LDFLAGS = -L../libdwarfparser/ -ldwarfparser -ldwarf -lelf  -L/home/kittel/projekte/libvmi/libdwarf/libdwarf/
