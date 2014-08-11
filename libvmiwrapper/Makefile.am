# @configure_input@
AM_CXXFLAGS = -Wall -Wextra -Werror -fpic -std=c++1y -I $(top_builddir)/ -I $(top_builddir)/common


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

lib_LTLIBRARIES = libvmiwrapper.la

libvmiwrapper_la_SOURCES =	libvmiwrapper.cpp \
							vmiinstance.cpp \
							vmiexception.cpp

#libvmiwrapper_la_HEADERS =	libvmiwrapper.h \
#							vmiinstance.h \
#							vmiexception.h

libvmiwrapper_la_CPPFLAGS = -I /home/kittel/projekte/code/libvmi/libvmi/ \
							-I /home/kittel/projekte/code/libdwarf/libdwarf/ \
							-ggdb \
							$(GLIB_CFLAGS) 
#libvmiwapper_la_LDFLAGS  = -ldwarf -lelf  -L/home/kittel/projekte/libvmi/libdwarf/libdwarf/
