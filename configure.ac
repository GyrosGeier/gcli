# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([gcli],[2.1.0-devel],[nsonack@herrhotzenplotz.de],[gcli],[https://gitlab.com/herrhotzenplotz/gcli/])
AM_INIT_AUTOMAKE([1.0 foreign subdir-objects dist-bzip2 dist-xz -Wall])

dnl Release Date.
PACKAGE_DATE="UNRELEASED"
AC_SUBST([PACKAGE_DATE])

dnl Silent by default.
AM_SILENT_RULES([yes])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

AM_PROG_AR

LT_INIT([pic-only])
AC_SUBST([LIBTOOL_DEPS])

AC_PROG_CC([cc clang gcc cl])
AC_LANG([C])
AC_PROG_YACC
AC_PROG_LEX(noyywrap)

PKG_PROG_PKG_CONFIG

dnl Use ccache if found
CCACHE=""
AC_CHECK_PROG([CCACHE], [ccache], [ccache])
if ! test -z "${CCACHE}"; then
	CC="${CCACHE} ${CC}"
fi

dnl Go looking for libcurl
OPT_LIBCURL=check
AC_ARG_WITH([libcurl],
[AS_HELP_STRING([--with-libcurl],
                [Give an alternate path to libcurl.])],
OPT_LIBCURL=$withval
)

AS_IF([test "x$OPT_LIBCURL" = "xno"],
      [AC_MSG_ERROR([--with-libcurl must not be disabled])])
AS_IF([test "x$OPT_LIBCURL" = "xcheck" || test "x$OPT_LIBCURL" = "xyes"],
      [PKG_CHECK_MODULES([LIBCURL], [libcurl],,[AC_MSG_ERROR([Could not find libcurl])])
       CFLAGS="$LIBCURL_CFLAGS $CFLAGS"
       LDFLAGS="$LIBCURL_LIBS $LDFLAGS"],
      [CPPFLAGS="-I$OPT_LIBCURL/include $CPPFLAGS"
       LDFLAGS="-L$OPT_LIBCURL/lib $LDFLAGS"])
AC_CHECK_HEADER([curl/curl.h],,[AC_MSG_ERROR([Cannot find libcurl headers])])
AC_CHECK_LIB([curl],[curl_easy_init],,[AC_MSG_ERROR([-lcurl doesn not contain curl_easy_init])])

dnl For the test suite we require libatf-c and Kyua
OPT_LIBATFC=check
AC_ARG_WITH([libatf-c],
[AS_HELP_STRING([--with-libatf-c],
                [Give an alternate path to libatf-c.])],
OPT_LIBATFC=$withval
)

HAVE_ATFC=no
if test "x$OPT_LIBATFC" = "xcheck" || test "x$OPT_LIBATFC" = "xyes"
then
        PKG_CHECK_MODULES([LIBATFC], [atf-c], [HAVE_ATFC=yes],[HAVE_ATFC=no])
        CFLAGS="$LIBATFC_CFLAGS $CFLAGS"
        LDFLAGS="$LIBATFC_LIBS $LDFLAGS"
elif test "x$OPT_LIBATFC" = "xno"
then
        HAVE_ATFC=no
else
        CPPFLAGS="-I$OPT_LIBATFC/include $CPPFLAGS"
        LDFLAGS="-L$OPT_LIBATFC/lib $LDFLAGS"
        HAVE_ATFC=yes
fi

AS_IF([test "x$HAVE_ATFC" = "xyes"],
      [AC_CHECK_HEADER([atf-c.h],,[AC_MSG_ERROR([Cannot find libatf-c headers])])
       AC_CHECK_LIB([atf-c],[atf_no_error],,[AC_MSG_ERROR([-latf-c doesn not contain atf_no_error])])
       HAVE_ATFC=yes])

AC_CHECK_PROG([KYUA], [kyua], [kyua])
HAVE_TESTS=0
AS_IF([test -z "$KYUA" || test "x$HAVE_ATFC" = xno],
      [HAVE_TESTS=no],
      [HAVE_TESTS=yes])
AC_SUBST([HAVE_TESTS])
AM_CONDITIONAL([HAVE_TESTS], [test "x$HAVE_TESTS" = xyes])

dnl Go looking for headers that may define getopt_long
AC_CHECK_HEADERS([getopt.h unistd.h])
dnl Other headers that we require
AC_CHECK_HEADERS([string.h signal.h sys/wait.h unistd.h sys/mman.h sys/types.h sys/queue.h],
                 ,
                 [AC_MSG_ERROR([Required header not found])])

dnl Check that the function is defined somewhere.
AC_CHECK_FUNC([getopt_long])

dnl Host OS name in various files
AC_DEFINE_UNQUOTED([HOSTOS], ["$host"], [Define to the triplet of the host operating system])

dnl Generate and substitute various files
AC_CONFIG_FILES([Makefile
                 libgcli.pc
                 docs/gcli-api.1
                 docs/gcli-comment.1
                 docs/gcli-config.1
                 docs/gcli-forks.1
                 docs/gcli-gists.1
                 docs/gcli-issues.1
                 docs/gcli-labels.1
                 docs/gcli-milestones.1
                 docs/gcli-pipelines.1
                 docs/gcli-pulls.1
                 docs/gcli-releases.1
                 docs/gcli-repos.1
                 docs/gcli-snippets.1
                 docs/gcli-status.1
                 docs/gcli.1])

dnl Technically only needed if tests are enabled but this doesn't
dnl hurt.
TESTSRCDIR="$(realpath ${srcdir}/tests)"
AC_SUBST([TESTSRCDIR])
AC_DEFINE_UNQUOTED([TESTSRCDIR], ["$TESTSRCDIR"], [Directory to the test sources])

AC_CONFIG_FILES([tests/Kyuafile])

AC_OUTPUT
