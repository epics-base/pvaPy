# ===========================================================================
#       http://www.gnu.org/software/autoconf-archive/ax_epics_base.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_EPICS4([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   Test for the EPICS4 libraries of a particular version (or newer)
#
#   If no path to the installed epics4 directory is given, the macro 
#   evaluates $EPICS4_DIR environment variable and searches under
#   /usr/local/epics4 directory
#
#   This macro sets:
#
#     HAVE_EPICS4
#     EPICS4_DIR
#
# LICENSE
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 1

DEFAULT_EPICS4_VERSION=4.3.0

AC_DEFUN([AX_EPICS4],

[AC_ARG_WITH([epics4],
    [AS_HELP_STRING([--with-epics4=EPICS4_DIR],
        [Use EPICS4 from the specified location])
    ],
    [
        if test -z "$withval"; then 
            AC_MSG_ERROR(--with-epics4-dir requires directory name)
        else
            ac_epics4_dir_path="$withval"
        fi
    ],
    [ac_epics4_dir_path=""])

    if test -z "$ac_epics4_dir_path"; then
        if test -z "$EPICS4_DIR"; then
            ac_epics4_dir_path="/usr/local/epics4"
        else
            ac_epics4_dir_path=$EPICS4_DIR
        fi
    fi
    AC_MSG_CHECKING(for EPICS4 directory $ac_epics4_dir_path)
    if ! test -d "$ac_epics4_dir_path"; then
        AC_MSG_ERROR($ac_epics4_dir_path is not a valid directory path)
    fi

    epics4_version_h="$ac_epics4_dir_path/pvAccessCPP/include/pv/pvaVersion.h"
    if ! test -f "$epics4_version_h"; then
        AC_MSG_RESULT([no])
        AC_MSG_ERROR(could not find valid EPICS4 installation in $ac_epics4_dir_path: no version header file $epics_version_h)
    fi
    AC_MSG_RESULT([yes])

    # define EPICS4_DIR
    EPICS4_DIR=$ac_epics4_dir_path

    # determine requested version
    epics4_version_req=ifelse([$1], , $DEFAULT_EPICS4_VERSION, $1)
    AC_MSG_CHECKING(for EPICS4 >= $epics4_version_req)
    epics4_version_req_major=`expr $epics4_version_req : '\([[0-9]]*\)'`
    epics4_version_req_minor=`expr $epics4_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
    epics4_version_req_sub_minor=`expr $epics4_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
    if test "x$epics4_version_req_sub_minor" = "x" ; then
        epics4_version_req_sub_minor="0"
        epics4_version_req_patch="0"
    else
        epics4_version_req_patch=`expr $epics4_version_req : '[[0-9]]*\.[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
        if test "x$epics4_version_req_patch" = "x" ; then
            epics4_version_req_patch="0"
        fi
    fi

    # At the moment there is no way of getting epics4 version
    # get epics4 version info
    #epics4_version_major=`grep EPICS_VERSION $epics4_version_h | head -1 | awk '{print $NF}'`
    #epics4_version_minor=`grep EPICS_REVISION $epics4_version_h | head -1 | awk '{print $NF}'`
    #epics4_version_sub_minor=`grep EPICS_MODIFICATION $epics4_version_h | head -1 | awk '{print $NF}'`
    #epics4_version_patch=`grep EPICS_PATCH_LEVEL $epics4_version_h | head -1 | awk '{print $NF}'`

    # test version
    want_epics4_version=`expr $epics4_version_req_major \* 1000000 \+  $epics4_version_req_minor \* 10000 \+ $epics4_version_req_sub_minor \* 100 \+ $epics4_version_req_patch`
    #have_epics4_version=`expr $epics4_version_major \* 1000000 \+  $epics4_version_minor \* 10000 \+ $epics4_version_sub_minor \* 100 \+ $epics4_version_patch`
    #if ! test $have_epics4_version -ge $want_epics4_version; then
    #    AC_MSG_RESULT([no])
    #    AC_MSG_ERROR("EPICS4 installation in $ac_epics4_dir_path is too old (required: $want_epics4_version, found: $have_epics4_version)")
    #fi
    AC_MSG_RESULT([yes])

    # test basic libraries
    AC_MSG_CHECKING(for usable EPICS4 libraries for $EPICS_OS_CLASS OS and host arhitecture $EPICS_HOST_ARCH)

    succeeded=no
    CPPFLAGS_SAVED="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS $EPICS_CPPFLAGS -I$EPICS_BASE/include -I$EPICS_BASE/include/os/$EPICS_OS_CLASS -I$EPICS4_DIR/pvDataCPP/include -I$EPICS4_DIR/pvAccessCPP/include"
    export CPPFLAGS

    LDFLAGS_SAVED="$LDFLAGS"
    LDFLAGS="$LDFLAGS $EPICS_LDFLAGS"
    LDFLAGS="$LDFLAGS -L$EPICS_BASE/lib/$EPICS_HOST_ARCH -L$EPICS4_DIR/pvDataCPP/lib/$EPICS_HOST_ARCH -L$EPICS4_DIR/pvAccessCPP/lib/$EPICS_HOST_ARCH -lpvData -lpvAccess -lCom"
    export LDFLAGS

    AC_REQUIRE([AC_PROG_CXX])
    AC_LANG_PUSH([C++])
        AC_LINK_IFELSE([AC_LANG_PROGRAM(
            [[
            #include "pv/pvAccess.h"
            #include "pv/pvData.h"
            ]],
            [[
            epics::pvData::FieldConstPtrArray fields;
            ]])
        ],[succeeded=yes],[succeeded=no])
    AC_LANG_POP([C++])

    if test "$succeeded" != "yes" ; then
        AC_MSG_RESULT([no])
        AC_MSG_ERROR(could not compile and link EPICS4 test code: check your EPICS4 installation)
    else
        AC_MSG_RESULT([yes])
        AC_DEFINE(HAVE_EPICS4,,[define if the EPICS4 libraries are available])
        AC_SUBST(EPICS4_DIR)
    fi

    CPPFLAGS="$CPPFLAGS_SAVED"
    LDFLAGS="$LDFLAGS_SAVED"

])
