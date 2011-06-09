# Process this file with autoconf to produce a configure script.

# Init autoconf and automake.
AC_PREREQ(2.59)
AC_INIT(SoTerrain, 0.5, blackhex@post.cz)
AC_CONFIG_SRCDIR([SoTerrainTest.cpp])
AC_LANG([C++])
AM_INIT_AUTOMAKE
AC_CONFIG_MACRO_DIR([m4])

# Subtitution variables
SO_TERRAIN_VERSION="0.5"
PR_PROFILER_VERSION="0.1"
AC_SUBST([SO_TERRAIN_VERSION])
AC_SUBST([PR_PROFILER_VERSION])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AM_PROG_LIBTOOL
AC_PROG_MAKE_SET

# Checks for header files.
AC_CHECK_HEADERS([sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_INT64_T
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_HEADER_STDC
AC_CHECK_FUNCS([gettimeofday putenv sqrt getopt])

# Set compiler flags accordingly to specified libraries
AC_ARG_WITH([coin], [AS_HELP_STRING([--with-coin],
 [set location of Coin library (default=/usr)])], [COIN_PATH=$withval],
 [COIN_PATH=/usr])
AC_SUBST([COIN_PATH])
AC_CHECK_FILE([$COIN_PATH/bin/coin-config], [],
  [AC_MSG_FAILURE([coin-config not found. Try to specify correct path to Coin3D using --with-coin=<coin_path> option.])])

#CXXFLAGS="`$COIN_PATH/bin/coin-config --cppflags` $CXXFLAGS"
#LDFLAGS="`$COIN_PATH/bin/coin-config --ldflags` $LDFLAGS"

AC_ARG_WITH([soqt], [AS_HELP_STRING([--with-soqt],
 [set location of SoQt library (default=/usr)])], [SOQT_PATH=$withval],
 [SOQT_PATH=/usr])
AC_SUBST([SOQT_PATH])
AC_CHECK_FILE([$SOQT_PATH/bin/soqt-config], [], 
  [AC_MSG_FAILURE([soqt-config not found. Try to specify correct path to SoQt libary using --with-soqt=<soqt_path> option.])])

# Set building variables.
CXXFLAGS="`$SOQT_PATH/bin/soqt-config --cppflags` -I./include/ -Wall \
  -Wno-long-long -ansi"
LDFLAGS="`$COIN_PATH/bin/coin-config --ldflags` \
 `$SOQT_PATH/bin/soqt-config --ldflags` \
 `$SOQT_PATH/bin/soqt-config --libs`"

# Set SoTerrain library.
AC_ARG_WITH([soterrain], [AS_HELP_STRING([--with-soterrain]),
  [set location of SoTerrain library (default=/usr)]],
  [SOTERRAIN_PATH=$withval],
  [SOTERRAIN_PATH=/usr])
AC_SUBST(SOTERRAIN_PATH)

LDFLAGS=" -L$SOTERRAIN_PATH/lib -lSoTerrain $LDFLAGS"
# TODO(irox): This is really pulling in libnav headers, add separate check/arg.
CXXFLAGS=" -I$SOTERRAIN_PATH/include $CXXFLAGS"

# Set SoTerrain includes.
AC_ARG_WITH([soterrain-includes], [AS_HELP_STRING([--with-soterrain-includes]),
  [set location of SoTerrain includes (default=/usr/include)]],
  [SOTERRAIN_INCLUDE_PATH=$withval],
  [SOTERRAIN_INCLUDE_PATH=/usr/include])
AC_SUBST(SOTERRAIN_INCLUDE_PATH)
CXXFLAGS=" -I $SOTERRAIN_INCLUDE_PATH $CXXFLAGS"

# Compilation with debugging informations.
AC_ARG_ENABLE(debug, [AS_HELP_STRING([--enable-debug],
  [compile with debugging informations (default=no)])])
if test "$enable_debug" = "yes"; then
  CXXFLAGS="-g3 -ggdb -DDEBUG $CXXFLAGS"
  LDFLAGS="-g3 -ggdb $LDFLAGS"
else
  CXXFLAGS="-O2 $CXXFLAGS"
  LDFLAGS="-O2 $LDFLAGS"
fi

# Compilation with profiling informations.
AC_ARG_ENABLE(profile, [AS_HELP_STRING([--enable-profile],
  [compile with profiling informations (default=no)])])
if test "$enable_debug" = "yes"; then
  CXXFLAGS="-pg $CXXFLAGS"
  LDFLAGS="-pg $LDFLAGS"
fi

# Compilation with own profiler.
AC_ARG_ENABLE(prprofiler, [AS_HELP_STRING([--enable-prprofiler],
 [compile with PrProfiler library (default=no)])])
if test "$enable_prprofiler" = "yes"; then
  CXXFLAGS="-DPROFILE $CXXFLAGS"
fi


# Checks for libraries.
AC_CHECK_LIB([Coin], [printf], [],
  [AC_MSG_FAILURE([Can't link against Coin library. Try to specify correct path to Coin libary using --with-coin=<coin_path> option.])])
AC_CHECK_LIB([SoQt], [printf], [],
  [AC_MSG_FAILURE([Can't link against SoQt library. Try to specify correct path to SoQt libary using --with-soqt=<soqt_path> option.])])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
