dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(multiget, 1.2.0)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC

WXCONFIG=wx-config
AC_ARG_WITH(wx-config,
[[  --with-wx-config=FILE
    Use the given path to wx-config when determining
    wxWidgets configuration; defaults to "wx-config"]],
[
    if test "$withval" != "yes" -a "$withval" != ""; then
        WXCONFIG=$withval
    fi
])
AC_MSG_CHECKING([wxWidgets version])
if wxversion=`$WXCONFIG --version`; then
    AC_MSG_RESULT([$wxversion])
else
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([wxWidgets is required. Try --with-wx-config.])
fi
 
WX_CPPFLAGS="`$WXCONFIG --cppflags`"
WX_CXXFLAGS="`$WXCONFIG --cxxflags`"
WX_LIBS="`$WXCONFIG --libs`"
 
AC_SUBST(WX_CPPFLAGS)
AC_SUBST(WX_CXXFLAGS)
AC_SUBST(WX_LIBS)

AC_PROG_CPP
AC_PROG_CXX


dnl ***************************************************************************
dnl Internatinalization
dnl ***************************************************************************
GETTEXT_PACKAGE=multiget
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.35.0])



AM_PROG_LIBTOOL



PKG_CHECK_MODULES(MULTIGET, [gtk+-2.0 libglade-2.0    ])
AC_SUBST(MULTIGET_CFLAGS)
AC_SUBST(MULTIGET_LIBS)


AC_OUTPUT([
Makefile
src/Makefile
po/Makefile.in
data/Makefile
data/multiget.desktop.in
])
