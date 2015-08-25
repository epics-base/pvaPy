# ===========================================================================
#       http://www.gnu.org/software/autoconf-archive/ax_pva_py.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PVA_PY([VERSION])
#
# DESCRIPTION
#
#   Configure pvaPy (python wrapper for PV Access) build.
#
# LICENSE
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 1

AC_DEFUN([AX_PVA_PY],

[AC_ARG_WITH([top],
    [AS_HELP_STRING([--with-top=TOP_DIR],
        [Path to TOP from the current directory])
    ],
    [
        if test -z "$withval"; then
            AC_MSG_ERROR(--with-top requires directory path)
        else
            ac_top_path="$withval"
        fi
    ],
    [ac_top_path=""])

    if test -z "$ac_top_path"; then
        if test -z "$TOP"; then
            ac_top_path="."
        else
            ac_top_path="$TOP"
        fi
    fi
    AC_MSG_CHECKING(for TOP directory $ac_top_path)
    if ! test -d "$ac_top_path"; then
        AC_MSG_ERROR($ac_top_path is not a valid directory path)
    fi
    if ! test -d "$ac_top_path/configure" -a -f "$ac_top_path/Makefile"; then
        AC_MSG_ERROR($ac_top_path does not point to an EPICS top directory)
    fi
    AC_MSG_RESULT([yes])

    #
    PVA_PY_TOP="$ac_top_path"
    current_dir=`pwd`
    cd $PVA_PY_TOP > /dev/null && PVA_PY_TOP=`pwd` && cd $current_dir > /dev/null

    # check for existing RELEASE.local file
    release_local=$PVA_PY_TOP/configure/RELEASE.local
    AC_MSG_CHECKING(for existing pvaPy $release_local file)
    if test -f $release_local; then
        AC_MSG_RESULT([yes])
        AC_MSG_ERROR(you must remove existing $release_local file in order to recreate pvaPy configuration)
    else
        AC_MSG_RESULT([no])
    fi

    # check for existing CONFIG_SITE.local file
    config_site_local=$PVA_PY_TOP/configure/CONFIG_SITE.local
    AC_MSG_CHECKING(for existing pvaPy $config_site_local file)
    if test -f $config_site_local; then
        AC_MSG_RESULT([yes])
        AC_MSG_ERROR(you must remove existing $config_site_local file in order to recreate pvaPy configuration)
    else
        AC_MSG_RESULT([no])
    fi

    # check for boost libraries
    AC_MSG_CHECKING(for boost)
    if test -z $BOOST_DIR; then
        AC_MSG_RESULT([no])
        AC_MSG_ERROR(cannot find boost libraries: BOOST_DIR is not defined)
    else
        AC_MSG_RESULT([yes])
    fi

    # check for boost python library
    AC_MSG_CHECKING(for boost python)
    if test -z $BOOST_PYTHON_LIB; then
        AC_MSG_RESULT([no])
        AC_MSG_ERROR(cannot find boost python library: BOOST_PYTHON_LIB is not defined)
    else
        AC_MSG_RESULT([yes])
    fi

    # check for python library
    AC_MSG_CHECKING(for python development library)
    if test -z "$PYTHON_CPPFLAGS"; then
        AC_MSG_RESULT([no])
        AC_MSG_ERROR(cannot find python development library: PYTHON_CPPFLAGS is not defined)
    else
        if test -z "$PYTHON_LDFLAGS"; then
            AC_MSG_RESULT([no])
            AC_MSG_ERROR(cannot find python development library: PYTHON_LDFLAGS is not defined)
        else
            AC_MSG_RESULT([yes])
        fi
    fi

    # check for epics base
    AC_MSG_CHECKING(for epics base)
    if test -z $EPICS_BASE; then
        AC_MSG_RESULT([no])
        AC_MSG_ERROR(cannot find epics base: EPICS_BASE is not defined)
    else
        AC_MSG_RESULT([yes])
    fi

    # check for epics4 
    AC_MSG_CHECKING(for epics4)
    if test -z $EPICS4_DIR; then
        AC_MSG_RESULT([no])
        AC_MSG_ERROR(cannot find epics4: EPICS4_DIR is not defined)
    else
        AC_MSG_RESULT([yes])
    fi

    # create RELEASE.local
    echo "PVACLIENT = $PVACLIENTCPP_DIR" >> $release_local
    echo "PVACCESS = $PVACCESSCPP_DIR" >> $release_local
    echo "NORMATIVETYPES = $NORMATIVETYPESCPP_DIR" >> $release_local
    echo "PVDATA = $PVDATACPP_DIR" >> $release_local
    echo "EPICS_BASE = $EPICS_BASE" >> $release_local
    AC_MSG_NOTICE([created $release_local file])

    # create CONFIG_SITE.local
    echo "PVA_PY_CPPFLAGS = $BOOST_CPPFLAGS $PYTHON_CPPFLAGS" >> $config_site_local
    echo "PVA_PY_LDFLAGS = $BOOST_LDFLAGS $PYTHON_LDFLAGS" >> $config_site_local
    echo "PVA_PY_SYS_LIBS = $BOOST_PYTHON_LIB" >> $config_site_local
    echo "PVA_API_VERSION = $PVA_API_VERSION" >> $config_site_local
    echo "PVA_RPC_API_VERSION = $PVA_RPC_API_VERSION" >> $config_site_local
    echo "PYTHON_VERSION := \$(shell python -c 'import sys; print sys.version[[:3]]')" >> $config_site_local
    AC_MSG_NOTICE([created $config_site_local file])
])
