#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([Zonda], [1.0], [jizhao.fanjz@alibaba-inc.com])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([common/thread/ThreadLock.cpp])
AM_INIT_AUTOMAKE([foreign])
AC_PREFIX_DEFAULT([`pwd`])
AC_CONFIG_HEADER([config.h])
AC_LANG([C++])
AC_CONFIG_FILES([
Makefile
common/Makefile
common/test/Makefile
samples/Makefile
samples/echo_service/Makefile
samples/seq_service/Makefile
samples/client/Makefile
route_admin/Makefile
])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for g++ c++0x support
CUR_CXXFLAGS=$CXXFLAGS
CXXFLAGS="-std=c++0x"
AC_COMPILE_IFELSE([
	int main()
	{   
		return 0;
	}   
	],  
	[ZONDA_CXXFLAG="$ZONDA_CXXFLAG -std=c++0x"],
	[CXXFLAGS=$CUR_CXXFLAGS]
)


# Checks for libraries.
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h memory.h stdlib.h string.h])
AC_CHECK_HEADERS([unordered_set unordered_map])
AC_CHECK_HEADERS([ext/hash_fun.h ext/stl_hash_fun.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([isascii memset regcomp strchr strncasecmp strstr strtol])



AC_SUBST([CXXFLAGS], [${ZONDA_CXXFLAG}])
AC_SUBST([LDADD], [${ZONDA_LDADD}])

AC_OUTPUT
