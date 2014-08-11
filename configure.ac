AC_INIT([libdwarfparser], [0.1], [thomas@kittel.it])
AC_PREREQ([2.65])

AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([-Wall])
AM_PROG_AR
LT_INIT
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_RANLIB
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
                                                                                
AC_CHECK_LIB(vmi, vmi_init, [], [echo "Error! You need to have libavl around."; exit 1])
AC_CHECK_HEADERS([libvmi/libvmi.h]) 

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([libdwarfparser/Makefile])
AC_CONFIG_FILES([libvmiwrapper/Makefile])
AC_CONFIG_FILES([tests/Makefile])

AC_OUTPUT
