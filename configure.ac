#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([linnya], [2.0.0], [http://www.linnya.org])
AM_INIT_AUTOMAKE(linnya, 2.0.0)
AC_CONFIG_SRCDIR([src/app.h])
AC_CONFIG_HEADERS([config.h])

# Set of available languages.
ALL_LINGUAS="zh_CN zh_TW zh_HK"

# Checks for programs.
${CXXFLAGS="-Wall -g -O2"}
AC_PROG_CC
AC_ISC_POSIX
CFLAGS="$CFLAGS -DINSTALL_PATH=\\\"$datadir/linnya/\\\""

#check for gtk-doc
#GTK_DOC_CHECK([1.14],[--flavour no-tmpl])

# Checks for libraries.
PKG_CHECK_MODULES([SQLITE3], [sqlite3])
PKG_CHECK_MODULES([GSTREAMER], [gstreamer-0.10 gstreamer-base-0.10 gstinterfaces-0.10])
PKG_CHECK_MODULES([GTK3], [gtk+-3.0 >= 3.0.0, gthread-2.0])
PKG_CHECK_MODULES([CURL], [libcurl >= 7.0])

# Checks for header files.
AC_CHECK_HEADERS([libintl.h locale.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_MALLOC

# Add the languages which your application supports to po/LINGUAS
GETTEXT_PACKAGE=linnya
AM_GNU_GETTEXT_VERSION([0.18.1])
AM_GNU_GETTEXT([external])

# Check for st_blksize in struct stat
AC_ST_BLKSIZE

# m4
LT_INIT
AC_CONFIG_MACRO_DIR([m4])

AC_OUTPUT([Makefile\
	po/Makefile.in\
	src/Makefile\
	src/cox/Makefile\
	src/uix/Makefile\
	src/pix/Makefile\
	src/pix/inf/Makefile\
	src/pix/opc/Makefile\
	src/pix/lrc/Makefile\
	src/pix/dnc/Makefile\
	src/pix/vdo/Makefile\
	data/Makefile\
	data/icon/Makefile\
	data/theme/Makefile\
	data/theme/default/Makefile\
	data/theme/dark/Makefile\
	data/theme/dark/sssbg/Makefile\
	data/theme/dark/gtk-3.0/Makefile\
	data/theme/dark/gtk-3.0/linnya_img/Makefile])
