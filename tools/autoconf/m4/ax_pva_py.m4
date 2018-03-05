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

#serial 3

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

    # check for boost python numpy library
    AC_MSG_CHECKING(for boost python numpy)
    if test $HAVE_BOOST_PYTHON_NUM_PY -eq 1 ; then
        AC_MSG_RESULT([yes])
    else
        AC_MSG_RESULT([no])
    fi

    # check for boost numpy library
    AC_MSG_CHECKING(for boost numpy)
    if test $HAVE_BOOST_NUM_PY -eq 1 ; then
        AC_MSG_RESULT([yes])
    else
        AC_MSG_RESULT([no])
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

    PVA_PY_LDFLAGS="$BOOST_LDFLAGS $PYTHON_LDFLAGS $BOOST_NUM_PY_LDFLAGS $BOOST_PYTHON_NUM_PY_LDFLAGS"
    PVA_PY_LD_LIBRARY_PATH=`echo $PVA_PY_LDFLAGS | sed 's?-L??g' | sed 's? ?:?g' | sed 's?:-l.*:?:?g' | sed 's?:-l.*??g'`
    AC_MSG_NOTICE([Determined BOOST_PYTHON_NUM_PY_LDFLAGS: $BOOST_PYTHON_NUM_PY_LDFLAGS])
    AC_MSG_NOTICE([Determined BOOST_NUM_PY_LDFLAGS: $BOOST_NUM_PY_LDFLAGS])
    AC_MSG_NOTICE([Determined BOOST_LDFLAGS: $BOOST_LDFLAGS])
    AC_MSG_NOTICE([Determined PYTHON_LDFLAGS: $PYTHON_LDFLAGS])
    AC_MSG_NOTICE([Determined PVA_PY_LDFLAGS: $PVA_PY_LDFLAGS])
    AC_MSG_NOTICE([Determined PVA_PY_LD_LIBRARY_PATH: $PVA_PY_LD_LIBRARY_PATH])

    # create RELEASE.local
    if ! test -z $PVACLIENTCPP_DIR; then
        echo "PVACLIENT = $PVACLIENTCPP_DIR" >> $release_local
    fi
    if ! test -z $PVDATABASECPP_DIR; then
        echo "PVDATABASE = $PVDATABASECPP_DIR" >> $release_local
    fi
    if ! test -z $NORMATIVETYPESCPP_DIR; then
        echo "NORMATIVETYPES = $NORMATIVETYPESCPP_DIR" >> $release_local
    fi
    echo "PVACCESS = $PVACCESSCPP_DIR" >> $release_local
    echo "PVDATA = $PVDATACPP_DIR" >> $release_local
    echo "EPICS_BASE = $EPICS_BASE" >> $release_local
    AC_MSG_NOTICE([Created $release_local file])

    # create CONFIG_SITE.local
    PVA_PY_MAJOR_MINOR_VERSION=`python$PYTHON_VERSION -c 'import sys; print(sys.version[[:3]])'`
    PVA_PY_PYTHON=`which python$PYTHON_VERSION`
    PVA_PY_PYTHON_DIR=`dirname $PVA_PY_PYTHON`
    PVA_PY_HOST_ARCH=`$EPICS_BASE/startup/EpicsHostArch.pl`
    PVA_PY_PYTHONPATH=$PVA_PY_TOP/lib/python/$PVA_PY_MAJOR_MINOR_VERSION/$PVA_PY_HOST_ARCH

    echo "PVA_PY_CPPFLAGS = $BOOST_CPPFLAGS $PYTHON_CPPFLAGS $BOOST_NUM_PY_CPPFLAGS $BOOST_PYTHON_NUM_PY_CPPFLAGS" >> $config_site_local
    echo "PVA_PY_LDFLAGS = $BOOST_LDFLAGS $PYTHON_LDFLAGS $BOOST_NUM_PY_LDFLAGS $BOOST_PYTHON_NUM_PY_LDFLAGS" >> $config_site_local
    echo "PVA_PY_SYS_LIBS = $BOOST_PYTHON_LIB $BOOST_NUM_PY_LIBS $BOOST_PYTHON_NUM_PY_LIBS" >> $config_site_local
    echo "PVA_API_VERSION = $PVA_API_VERSION" >> $config_site_local
    echo "PVA_RPC_API_VERSION = $PVA_RPC_API_VERSION" >> $config_site_local
    echo "HAVE_BOOST_NUM_PY = $HAVE_BOOST_NUM_PY" >> $config_site_local
    echo "HAVE_BOOST_PYTHON_NUM_PY = $HAVE_BOOST_PYTHON_NUM_PY" >> $config_site_local
    echo "PYTHON_VERSION = $PVA_PY_MAJOR_MINOR_VERSION" >> $config_site_local
    echo "PVA_PY_PYTHON = $PVA_PY_PYTHON" >> $config_site_local
    echo "PVA_PY_PYTHONPATH = $PVA_PY_PYTHONPATH" >> $config_site_local
    echo "PVA_PY_LD_LIBRARY_PATH = $PVA_PY_LD_LIBRARY_PATH" >> $config_site_local
    echo "PVA_PY_EPICS_BASE = $EPICS_BASE" >> $config_site_local
    echo "PVA_PY_EPICS4_DIR = $EPICS4_DIR" >> $config_site_local
    echo "PVA_PY_HOST_ARCH = $PVA_PY_HOST_ARCH" >> $config_site_local
    echo "PVA_PY_SETUP_SH = $PWD/bin/$PVA_PY_HOST_ARCH/pvapy_setup_full.$PVA_PY_MAJOR_MINOR_VERSION.sh" >> $config_site_local
    AC_MSG_NOTICE([Created $config_site_local file])

    # create setup files
    LOCAL_SETUP_DIR=$PVA_PY_TOP/bin/$PVA_PY_HOST_ARCH
    mkdir -p $LOCAL_SETUP_DIR

    # create SETUP_PYTHONPATH.SH
    setup_sh=$PVA_PY_TOP/configure/SETUP_PYTHONPATH.SH
    setup_sh_local=$LOCAL_SETUP_DIR/pvapy_setup_pythonpath.$PVA_PY_MAJOR_MINOR_VERSION.sh
    eval "cat $setup_sh | sed 's?PVA_PY_PYTHON_DIR?$PVA_PY_PYTHON_DIR?g' | sed 's?PVA_PY_PYTHONPATH?$PVA_PY_PYTHONPATH?g' | sed 's?PVA_PY_LD_LIBRARY_PATH?$PVA_PY_LD_LIBRARY_PATH?g' > $setup_sh_local"
    AC_MSG_NOTICE([Created $setup_sh_local file])
    
    # create SETUP_FULL.SH
    setup_sh=$PVA_PY_TOP/configure/SETUP_FULL.SH
    setup_sh_local=$LOCAL_SETUP_DIR/pvapy_setup_full.$PVA_PY_MAJOR_MINOR_VERSION.sh
    eval "cat $setup_sh | sed 's?PVA_PY_PYTHON_DIR?$PVA_PY_PYTHON_DIR?g' | sed 's?PVA_PY_PYTHONPATH?$PVA_PY_PYTHONPATH?g' | sed 's?PVA_PY_LD_LIBRARY_PATH?$PVA_PY_LD_LIBRARY_PATH?g' > $setup_sh_local"
    AC_MSG_NOTICE([Created $setup_sh_local file])
    
    # create SETUP_PYTHONPATH.CSH
    setup_csh=$PVA_PY_TOP/configure/SETUP_PYTHONPATH.CSH
    setup_csh_local=$LOCAL_SETUP_DIR/pvapy_setup_pythonpath.$PVA_PY_MAJOR_MINOR_VERSION.csh
    eval "cat $setup_csh | sed 's?PVA_PY_PYTHON_DIR?$PVA_PY_PYTHON_DIR?g' | sed 's?PVA_PY_PYTHONPATH?$PVA_PY_PYTHONPATH?g' | sed 's?PVA_PY_LD_LIBRARY_PATH?$PVA_PY_LD_LIBRARY_PATH?g' > $setup_csh_local"
    AC_MSG_NOTICE([Created $setup_csh_local file])
    
    # create SETUP_FULL.CSH
    setup_csh=$PVA_PY_TOP/configure/SETUP_FULL.CSH
    setup_csh_local=$LOCAL_SETUP_DIR/pvapy_setup_full.$PVA_PY_MAJOR_MINOR_VERSION.csh
    eval "cat $setup_csh | sed 's?PVA_PY_PYTHON_DIR?$PVA_PY_PYTHON_DIR?g' | sed 's?PVA_PY_PYTHONPATH?$PVA_PY_PYTHONPATH?g' | sed 's?PVA_PY_LD_LIBRARY_PATH?$PVA_PY_LD_LIBRARY_PATH?g' > $setup_csh_local"
    AC_MSG_NOTICE([Created $setup_csh_local file])
])
