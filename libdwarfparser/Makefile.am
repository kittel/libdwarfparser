# @configure_input@
AM_CXXFLAGS = -ggdb -pg -Wall -Wextra -Werror -fpic -std=c++14 -I $(top_builddir)/ -I $(top_builddir)/common -O3


# Package-specific substitution variables
package = @PACKAGE_NAME@
version = @PACKAGE_VERSION@
tarname = @PACKAGE_TARNAME@
distdir = $(tarname)-$(version)

# Prefix-specific substitution variables
prefix      = @prefix@
exec_prefix = @exec_prefix@
bindir      = @bindir@
srcdir      = @srcdir@

lib_LTLIBRARIES = libdwarfparser.la

pkginclude_HEADERS = libdwarfparser.h \
                     array.h \
                     basetype.h \
                     consttype.h \
                     dwarfexception.h \
                     dwarfparser.h \
                     enum.h \
                     funcpointer.h \
                     function.h \
                     instance.h \
                     pointer.h \
                     refbasetype.h \
                     referencingtype.h \
                     struct.h \
                     structured.h \
                     structuredmember.h \
                     symbol.h \
                     symbolmanager.h \
                     typedef.h \
                     union.h \
                     variable.h

libdwarfparser_la_SOURCES = array.cpp \
                            basetype.cpp \
                            consttype.cpp \
                            dwarfexception.cpp \
                            dwarfparser.cpp \
                            enum.cpp \
                            funcpointer.cpp \
                            function.cpp \
                            instance.cpp \
                            pointer.cpp \
                            refbasetype.cpp \
                            referencingtype.cpp \
                            struct.cpp \
                            structured.cpp \
                            structuredmember.cpp \
                            symbol.cpp \
                            symbolmanager.cpp \
                            typedef.cpp \
                            union.cpp \
                            variable.cpp

libdwarfparser_la_CPPFLAGS = -ggdb \
                             $(GLIB_CFLAGS)
