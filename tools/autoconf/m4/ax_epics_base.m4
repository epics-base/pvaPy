# ===========================================================================
#       http://www.gnu.org/software/autoconf-archive/ax_epics_base.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_EPICS_BASE([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   Test for the EPICS base libraries of a particular version (or newer)
#
#   If no path to the installed epics library is given, the macro 
#   evaluates $EPICS_BASE environment variable and searches
#   under /usr/local/epics/base.
#
#   This macro sets:
#
#     HAVE_EPICS
#     EPICS_BASE
#     EPICS_HOST_ARCH
#     EPICS_CPPFLAGS
#     EPICS_LDFLAGS
#
# LICENSE
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 1

DEFAULT_EPICS_VERSION=3.14.12

AC_DEFUN([AX_EPICS_BASE],

[AC_ARG_WITH([epics-base],
    [AS_HELP_STRING([--with-epics-base=EPICS_BASE],
        [Use EPICS from the specified location])
    ],
    [
        if test -z "$withval"; then 
            AC_MSG_ERROR(--with-epics-base requires directory name)
        else
            ac_epics_base_path="$withval"
        fi
    ],
    [ac_epics_base_path=""])

    if test -z "$ac_epics_base_path"; then
        if test -z "$EPICS_BASE"; then
            ac_epics_base_path="/usr/local/epics/base"
        else
            ac_epics_base_path=$EPICS_BASE
        fi
    fi
    AC_MSG_CHECKING(for EPICS base directory $ac_epics_base_path)
    if ! test -d "$ac_epics_base_path"; then
        AC_MSG_ERROR($ac_epics_base_path is not a valid directory path)
    fi

    # save compile/link flags, set lang to C++
    AC_REQUIRE([AC_PROG_CXX])
    AC_LANG_PUSH([C++])
    SAVED_CPPFLAGS="$CPPFLAGS"
    SAVED_LDFLAGS="$LDFLAGS"

    epics_version_h="$ac_epics_base_path/include/epicsVersion.h"
    if ! test -f "$epics_version_h"; then
        AC_MSG_RESULT([no])
        AC_MSG_ERROR(could not find valid EPICS base installation in $ac_epics_base_path: no version header file $epics_version_h)
    fi
    AC_MSG_RESULT([yes])

    # determine epics host arch
    if test -z "$EPICS_HOST_ARCH"; then
        EPICS_HOST_ARCH=`$ac_epics_base_path/startup/EpicsHostArch`
    fi
    if test -z "$EPICS_HOST_ARCH"; then
        AC_MSG_ERROR(could not determine EPICS host architecture)
    fi

    # define EPICS_BASE
    EPICS_BASE=$ac_epics_base_path

    # determine required version
    epics_version_req=ifelse([$1], , $DEFAULT_EPICS_VERSION, $1)
    AC_MSG_CHECKING(for EPICS Base version >= $epics_version_req)
    req_epics_version=`expr $epics_version_req : '\([[0-9]]*\)'`
    req_epics_revision=`expr $epics_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
    req_epics_modification=`expr $epics_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
    if test "x$req_epics_modification" = "x" ; then
        req_epics_modification="0"
        req_epics_patch_level="0"
    else
        epics_version_req_patch=`expr $epics_version_req : '[[0-9]]*\.[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
        if test "x$req_epics_patch_level" = "x" ; then
            req_epics_patch_level="0"
        fi
    fi

    # Search include directory only, sufficient for epicsVersion.h
    EPICS_CPPFLAGS="$CMD_CPPFLAGS -I$EPICS_BASE/include"
    CPPFLAGS="$SAVED_CPPFLAGS $EPICS_CPPFLAGS"

    # check epics version
    AC_PREPROC_IFELSE([AC_LANG_SOURCE([[
        #include "epicsVersion.h"
        #ifndef VERSION_INT
        #  define VERSION_INT(V,R,M,P) (((V)<<24) | ((R)<<16) | ((M)<<8) | (P))
        #  define EPICS_VERSION_INT VERSION_INT(EPICS_VERSION,EPICS_REVISION,EPICS_MODIFICATION,EPICS_PATCH_LEVEL)
        #endif
        #if EPICS_VERSION_INT < VERSION_INT($req_epics_version,$req_epics_revision,$req_epics_modification,$req_epics_patch_level)
        #  error too old
        #endif
    ]])],[AC_MSG_RESULT([yes])],[
        AC_MSG_RESULT([no])
        AC_MSG_ERROR("EPICS Base installation in $ac_epics_base_path is too old (required: $epics_version_req)")
    ])

    # test basic libraries
    # need to determine OS class first
    epics_host_arch_main=`echo $EPICS_HOST_ARCH | cut -f1 -d'-'`
    EPICS_OS_CLASS=`grep OS_CLASS $EPICS_BASE/configure/os/CONFIG.*Common.${epics_host_arch_main}* | head -1 | awk '{print $NF}'`
    if test -d "$EPICS_BASE/include/os/$EPICS_OS_CLASS"; then
        EPICS_CPPFLAGS="$EPICS_CPPFLAGS -I$EPICS_BASE/include/os/$EPICS_OS_CLASS"
    fi

    # need to determine CMPLR_CLASS, which is *hard*, so
    # TODO: replace these hardcoded settings with the correct one from the EPICS configure directory
    if test "$epics_host_arch_main" = "linux" ; then
        EPICS_CMPLR_CLASS=gcc
    elif test "$epics_host_arch_main" = "darwin" ; then
        EPICS_CMPLR_CLASS=clang
    fi
    AC_MSG_CHECKING(for EPICS Base libraries for $EPICS_HOST_ARCH)

    EPICS_CPPFLAGS="$EPICS_CPPFLAGS -I$EPICS_BASE/include/compiler/$EPICS_CMPLR_CLASS"
    export CPPFLAGS="$SAVED_CPPFLAGS $EPICS_CPPFLAGS"

    EPICS_LDFLAGS="-L$EPICS_BASE/lib/$EPICS_HOST_ARCH"
    export LDFLAGS="$SAVED_LDFLAGS $EPICS_LDFLAGS"

    export LIBS="-lca -lCom"

    AC_LINK_IFELSE([AC_LANG_PROGRAM(
        [[#include <cadef.h>]],
        [[ca_context_create(ca_enable_preemptive_callback);]])
    ],[succeeded=yes],[succeeded=no])

    if test "$succeeded" != "yes" ; then
        AC_MSG_RESULT([no])
        AC_MSG_ERROR(could not compile and link EPICS test code: check your EPICS base installation)
    else
        AC_MSG_RESULT([yes])
        AC_DEFINE(HAVE_EPICS, , [define if the EPICS base is available])
        AC_SUBST(EPICS_BASE)
        AC_SUBST(EPICS_HOST_ARCH)
        AC_SUBST(EPICS_CPPFLAGS)
        AC_SUBST(EPICS_LDFLAGS)
    fi

    # restore compile/link flags
    CPPFLAGS="$SAVED_CPPFLAGS"
    LDFLAGS="$SAVED_LDFLAGS"
    AC_LANG_POP([C++])
])
