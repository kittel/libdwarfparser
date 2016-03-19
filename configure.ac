AC_INIT([libdwarfparser], [0.1], [thomas@kittel.it])
AC_PREREQ([2.65])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
# use the C++ compiler for the following checks
AC_LANG([C++])
AM_CXXFLAGS="$AM_CXXFLAGS -std=c++14"

AM_INIT_AUTOMAKE([-Wall])
AM_PROG_AR
LT_INIT
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_INSTALL


AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h])
AC_CHECK_HEADER_STDBOOL
AC_FUNC_ERROR_AT_LINE
AC_TYPE_UINT8_T
AC_TYPE_INT64_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T


AC_CHECK_HEADERS([string])

PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.16])
AC_SUBST([GLIB_CFLAGS])
AC_SUBST([GLIB_LIBS])

dnl save libs
OLDLIBS=$LIBS

dnl check libelf library 
AC_CHECK_HEADER([libelf.h],[ac_cv_has_libelf=1],[ac_cv_has_libelf=0]) 
if [[ x"${ac_cv_has_libelf}" != x1 ]] 
then 
  AC_MSG_FAILURE([please install libelf development package]) 
fi 
AC_CHECK_LIB([vmi], [vmi_init], [], 
			 [AC_MSG_ERROR(["Error! You need to have libvmi around."])]
)
AC_CHECK_LIB([elf], [elf_begin], [], 
			 [AC_MSG_ERROR(["Error! You need to have libelf around."])]
)
AC_CHECK_HEADERS([libvmi/libvmi.h], [], 
				 [AC_MSG_ERROR(["Error! You need to have libvmi headers around."])]
)
AC_CHECK_LIB([dwarf], [dwarf_init], [], 
			 [AC_MSG_ERROR(["Error! You need to have libdwarf around."])], 
			 ["-lelf"]
)
AC_CHECK_HEADERS([libdwarf/libdwarf.h], [], 
				 [AC_MSG_ERROR(["Error! You need to have libdwarf headers around."])]
)

AC_CHECK_HEADERS([CppUTest/MemoryLeakDetectorMallocMacros.h])
AC_CHECK_HEADERS([CppUTest/TestHarness.h])

dnl Check for cython -> required for python bindings
AC_PATH_PROG([CYTHON], [cython])
AM_CONDITIONAL([HAVE_CYTHON], [test "$CYTHON"])

dnl Check for python -> required for python bindings
AM_PATH_PYTHON([2.0], 
			   [], 
			   [AC_MSG_WARN([Python not found. Python is required to build presage python binding. Python can be obtained from http://www.python.org])])
if test "$PYTHON" != :
then
    python_include_path=`$PYTHON -c "import distutils.sysconfig; print distutils.sysconfig.get_python_inc();"`
    AC_CHECK_HEADERS([${python_include_path}/Python.h],
                     [have_python_header=true],
                     [AC_MSG_WARN([Python.h header file not found. Python development files are required to build presage python binding. Python can be obtained from http://www.python.org])],
                     [])
fi
AM_CONDITIONAL([HAVE_PYTHON], [test "$PYTHON" != : -a "x$have_python_header" = "xtrue"])


AC_ARG_ENABLE([python-binding],
              AS_HELP_STRING([--disable-python-binding],
                             [build python binding (default=enabled)]),
              [ac_enable_python_binding=$enableval],
              [ac_enable_python_binding=yes])
if test "x$ac_enable_python_binding" = "xyes"
then
    if test ! "$CYTHON" -o ! "$PYTHON" != : -o ! "x$have_python_header" = "xtrue"
    then
        AC_MSG_WARN([Python bindings cannot be built. Ensure that Cython and Python packages are available.])
    fi
else
    AC_MSG_NOTICE([Python bindings will not be built.])
    AC_MSG_NOTICE([Enable Python binding module building with --enable-python-binding])
fi
AM_CONDITIONAL([ENABLE_PYTHON_BINDING], [test "x$ac_enable_python_binding" = "xyes"])


LIBS=$OLDLIBS

DX_INIT_DOXYGEN([$PACKAGE_NAME],[Doxyfile], [doc])
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(ON)
DX_PS_FEATURE(OFF)

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([libdwarfparser/Makefile])
AC_CONFIG_FILES([libdwarfparser/python/Makefile])
AC_CONFIG_FILES([libdwarfparser/python/setup.py])
AC_CONFIG_FILES([libvmiwrapper/Makefile])
AC_CONFIG_FILES([tests/Makefile])

AC_OUTPUT
