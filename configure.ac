#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_DEFINE(PACKAGE, "Swingxficgenerator")
AC_DEFINE(VERSION, "0.3.1")
AC_INIT(PACKAGE, VERSION, swingxficgenerator@gmail.com)
AM_INIT_AUTOMAKE("Swingxficgenerator", "0.3.1")
AM_CONFIG_HEADER(config.h)






AC_DEFINE_UNQUOTED(SPECSFILE,"${prefix}/[specs.all]","General")
AC_DEFINE_UNQUOTED(HOSTSFILE,"${prefix}/[bin/latest/hosts]", "General")
AC_DEFINE_UNQUOTED(STARTLISTENERFILENAME,"${prefix}/[bin/startlistener]", "General")
AC_DEFINE_UNQUOTED(ERRFILECONNFORKS,"${prefix}/[errfileconnforks]", "General")
AC_SUBST(SPECSFILE)
AC_SUBST(HOSTSFILE)
AC_SUBST(STARTLISTENERFILENAME)
AC_SUBST(ERRFILECONNFORKS)



# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
# Checks for libraries.

#AC_CHECK_LIB([libpcap])
# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
#patch begin
# Checks for programs.
# AC_PROG_CXX
# AC_PROG_CC
 AC_PROG_MAKE_SET
#
## Checks for libraries.
# FIXME: Replace `main' with a function in `-lmylib':
# AC_CHECK_LIB([mylib], [main])
#
## Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([netdb.h netinet/in.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
# AC_C_INLINE
# AC_HEADER_TIME
#
## Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([gethostbyname gettimeofday memset select socket strdup strstr])



#patch end
AC_CONFIG_FILES([Makefile
				 scripts/Makefile
				 src/Makefile])
AC_OUTPUT
