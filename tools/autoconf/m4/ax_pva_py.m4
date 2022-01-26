# ===========================================================================
#       http://www.gnu.org/software/autoconf-archive/ax_pva_py.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_PVAPY([VERSION])
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

#serial 5

AC_DEFUN([AX_PVAPY],

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
    PVAPY_TOP="$ac_top_path"
    current_dir=`pwd`
    cd $PVAPY_TOP > /dev/null && PVAPY_TOP=`pwd` && cd $current_dir > /dev/null

    # disabled check for existing RELEASE.local file
    release_local=$PVAPY_TOP/configure/RELEASE.local
    #AC_MSG_CHECKING(for existing pvaPy $release_local file)
    #if test -f $release_local; then
    #    AC_MSG_RESULT([yes])
    #    AC_MSG_ERROR(you must remove existing $release_local file in order to recreate pvaPy configuration)
    #else
    #    AC_MSG_RESULT([no])
    #fi

    # disabled check for existing CONFIG_SITE.local file
    config_site_local=$PVAPY_TOP/configure/CONFIG_SITE.local
    #AC_MSG_CHECKING(for existing pvaPy $config_site_local file)
    #if test -f $config_site_local; then
    #    AC_MSG_RESULT([yes])
    #    AC_MSG_ERROR(you must remove existing $config_site_local file in order to recreate pvaPy configuration)
    #else
    #    AC_MSG_RESULT([no])
    #fi

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
    if test $HAVE_BOOST_PYTHON_NUMPY -eq 1 ; then
        AC_MSG_RESULT([yes])
    else
        AC_MSG_RESULT([no])
    fi

    # check for boost numpy library
    AC_MSG_CHECKING(for boost numpy)
    if test $HAVE_BOOST_NUMPY -eq 1 ; then
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

    PVAPY_CPPFLAGS="$BOOST_CPPFLAGS $PYTHON_CPPFLAGS $BOOST_NUMPY_CPPFLAGS $BOOST_PYTHON_NUMPY_CPPFLAGS"
    PVAPY_CPPFLAGS=`echo $(printf "%s\n" $PVAPY_CPPFLAGS | sort -u)`
    PVAPY_LDFLAGS="$BOOST_LDFLAGS $PYTHON_LDFLAGS $BOOST_NUMPY_LDFLAGS $BOOST_PYTHON_NUMPY_LDFLAGS"
    PVAPY_LDFLAGS=`echo $(printf "%s\n" $PVAPY_LDFLAGS | sort -u)`
    PVAPY_LD_LIBRARY_PATH=`echo $PVAPY_LDFLAGS | sed 's?-L??g' | sed 's? ?:?g' | sed 's?:-l.*:?:?g' | sed 's?:-l.*??g'`

    PYTHON_MAJOR_VERSION=`python$PYTHON_VERSION -c "import sys; print(sys.version.split()[[0]].split('.')[[0]])"`
    PYTHON_MINOR_VERSION=`python$PYTHON_VERSION -c "import sys; print(sys.version.split()[[0]].split('.')[[1]])"`
    PYTHON_MAJOR_MINOR_VERSION=$PYTHON_MAJOR_VERSION.$PYTHON_MINOR_VERSION

    AC_MSG_NOTICE([Determined BOOST_PYTHON_NUMPY_CPPFLAGS: $BOOST_PYTHON_NUMPY_CPPFLAGS])
    AC_MSG_NOTICE([Determined BOOST_PYTHON_NUMPY_LDFLAGS: $BOOST_PYTHON_NUMPY_LDFLAGS])
    AC_MSG_NOTICE([Determined BOOST_NUMPY_CPPFLAGS: $BOOST_NUMPY_CPPFLAGS])
    AC_MSG_NOTICE([Determined BOOST_NUMPY_LDFLAGS: $BOOST_NUMPY_LDFLAGS])
    AC_MSG_NOTICE([Determined BOOST_CPPFLAGS: $BOOST_CPPFLAGS])
    AC_MSG_NOTICE([Determined BOOST_LDFLAGS: $BOOST_LDFLAGS])
    AC_MSG_NOTICE([Determined PYTHON_MAJOR_MINOR_VERSION: $PYTHON_MAJOR_MINOR_VERSION])
    AC_MSG_NOTICE([Determined PYTHON_CPPFLAGS: $PYTHON_CPPFLAGS])
    AC_MSG_NOTICE([Determined PYTHON_LDFLAGS: $PYTHON_LDFLAGS])

    AC_MSG_NOTICE([Determined PVAPY_CPPFLAGS: $PVAPY_CPPFLAGS])
    AC_MSG_NOTICE([Determined PVAPY_LDFLAGS: $PVAPY_LDFLAGS])
    AC_MSG_NOTICE([Determined PVAPY_LD_LIBRARY_PATH: $PVAPY_LD_LIBRARY_PATH])

    # create RELEASE.local
    if ! test -z $PVACLIENTCPP_DIR; then
        if test "$PVACLIENTCPP_DIR" != "$EPICS_BASE"; then
            echo "PVACLIENT = $PVACLIENTCPP_DIR" >> $release_local
        fi
    fi
    if ! test -z $PVDATABASECPP_DIR; then
        if test "$PVDATABASECPP_DIR" != "$EPICS_BASE"; then
            echo "PVDATABASE = $PVDATABASECPP_DIR" >> $release_local
        fi
    fi
    if ! test -z $NORMATIVETYPESCPP_DIR; then
        if test "$NORMATIVETYPESCPP_DIR" != "$EPICS_BASE"; then
            echo "NORMATIVETYPES = $NORMATIVETYPESCPP_DIR" >> $release_local
        fi
    fi
    if test "$PVACCESSCPP_DIR" != "$EPICS_BASE"; then
        echo "PVACCESS = $PVACCESSCPP_DIR" >> $release_local
    fi
    if test "$PVDATACPP_DIR" != "$EPICS_BASE"; then
        echo "PVDATA = $PVDATACPP_DIR" >> $release_local
    fi
    echo "EPICS_BASE = $EPICS_BASE" >> $release_local
    AC_MSG_NOTICE([Created $release_local file])

    # create CONFIG_SITE.local
    PVAPY_PYTHON=`which python$PYTHON_VERSION`
    PVAPY_PYTHON_DIR=`dirname $PVAPY_PYTHON`
    PVAPY_HOST_ARCH=$EPICS_HOST_ARCH
    PVAPY_PYTHONPATH=$PVAPY_TOP/lib/python/$PYTHON_MAJOR_MINOR_VERSION/$PVAPY_HOST_ARCH
    PVAPY_SPHINX_BUILD=`which sphinx-build 2> /dev/null`

    echo "PVAPY_CPPFLAGS = $PVAPY_CPPFLAGS" >> $config_site_local
    echo "PVAPY_LDFLAGS = $PVAPY_LDFLAGS" >> $config_site_local
    echo "PVAPY_SYS_LIBS = $BOOST_PYTHON_LIB $BOOST_NUMPY_LIBS $BOOST_PYTHON_NUMPY_LIBS" >> $config_site_local
    echo "PVA_API_VERSION = $PVA_API_VERSION" >> $config_site_local
    echo "PVA_RPC_API_VERSION = $PVA_RPC_API_VERSION" >> $config_site_local
    echo "HAVE_BOOST_NUMPY = $HAVE_BOOST_NUMPY" >> $config_site_local
    echo "HAVE_BOOST_PYTHON_NUMPY = $HAVE_BOOST_PYTHON_NUMPY" >> $config_site_local
    echo "PYTHON_VERSION = $PYTHON_MAJOR_MINOR_VERSION" >> $config_site_local
    echo "PVAPY_PYTHON = $PVAPY_PYTHON" >> $config_site_local
    echo "PVAPY_PYTHONPATH = $PVAPY_PYTHONPATH" >> $config_site_local
    echo "PVAPY_LD_LIBRARY_PATH = $PVAPY_LD_LIBRARY_PATH" >> $config_site_local
    echo "PVAPY_SPHINX_BUILD = $PVAPY_SPHINX_BUILD" >> $config_site_local
    echo "PVAPY_EPICS_BASE = $EPICS_BASE" >> $config_site_local
    echo "PVAPY_EPICS4_DIR = $EPICS4_DIR" >> $config_site_local
    echo "PVAPY_HOST_ARCH = $PVAPY_HOST_ARCH" >> $config_site_local
    echo "PVAPY_SETUP_SH = $PWD/bin/$PVAPY_HOST_ARCH/pvapy_setup_full.$PYTHON_MAJOR_MINOR_VERSION.sh" >> $config_site_local
    AC_MSG_NOTICE([Created $config_site_local file])

    # create setup files
    LOCAL_SETUP_DIR=$PVAPY_TOP/bin/$PVAPY_HOST_ARCH
    mkdir -p $LOCAL_SETUP_DIR

    # create SETUP_PYTHONPATH.SH
    setup_sh=$PVAPY_TOP/configure/SETUP_PYTHONPATH.SH
    setup_sh_local=$LOCAL_SETUP_DIR/pvapy_setup_pythonpath.$PYTHON_MAJOR_MINOR_VERSION.sh
    eval "cat $setup_sh | sed 's?PVAPY_PYTHON_DIR?$PVAPY_PYTHON_DIR?g' | sed 's?PVAPY_PYTHONPATH?$PVAPY_PYTHONPATH?g' | sed 's?PVAPY_LD_LIBRARY_PATH?$PVAPY_LD_LIBRARY_PATH?g' > $setup_sh_local"
    AC_MSG_NOTICE([Created $setup_sh_local file])
    
    # create SETUP_FULL.SH
    setup_sh=$PVAPY_TOP/configure/SETUP_FULL.SH
    setup_sh_local=$LOCAL_SETUP_DIR/pvapy_setup_full.$PYTHON_MAJOR_MINOR_VERSION.sh
    eval "cat $setup_sh | sed 's?PVAPY_PYTHON_DIR?$PVAPY_PYTHON_DIR?g' | sed 's?PVAPY_PYTHONPATH?$PVAPY_PYTHONPATH?g' | sed 's?PVAPY_LD_LIBRARY_PATH?$PVAPY_LD_LIBRARY_PATH?g' | sed 's?export PVAPY_SPHINX_BUILD.*?export PVAPY_SPHINX_BUILD=$PVAPY_SPHINX_BUILD?g' > $setup_sh_local"
    AC_MSG_NOTICE([Created $setup_sh_local file])
    
    # create SETUP_PYTHONPATH.CSH
    setup_csh=$PVAPY_TOP/configure/SETUP_PYTHONPATH.CSH
    setup_csh_local=$LOCAL_SETUP_DIR/pvapy_setup_pythonpath.$PYTHON_MAJOR_MINOR_VERSION.csh
    eval "cat $setup_csh | sed 's?PVAPY_PYTHON_DIR?$PVAPY_PYTHON_DIR?g' | sed 's?PVAPY_PYTHONPATH?$PVAPY_PYTHONPATH?g' | sed 's?PVAPY_LD_LIBRARY_PATH?$PVAPY_LD_LIBRARY_PATH?g' > $setup_csh_local"
    AC_MSG_NOTICE([Created $setup_csh_local file])
    
    # create SETUP_FULL.CSH
    setup_csh=$PVAPY_TOP/configure/SETUP_FULL.CSH
    setup_csh_local=$LOCAL_SETUP_DIR/pvapy_setup_full.$PYTHON_MAJOR_MINOR_VERSION.csh
    eval "cat $setup_csh | sed 's?PVAPY_PYTHON_DIR?$PVAPY_PYTHON_DIR?g' | sed 's?PVAPY_PYTHONPATH?$PVAPY_PYTHONPATH?g' | sed 's?PVAPY_LD_LIBRARY_PATH?$PVAPY_LD_LIBRARY_PATH?g' | sed 's?setenv PVAPY_SPHINX_BUILD.*?setenv PVAPY_SPHINX_BUILD $PVAPY_SPHINX_BUILD?g' > $setup_csh_local"
    AC_MSG_NOTICE([Created $setup_csh_local file])
])

    # configure pvaccess module
    cd $PVAPY_TOP/pvaccess > /dev/null 
    rm -f pvaccess.so
    ln -s $PVAPY_PYTHONPATH/pvaccess.so .
    cd $current_dir > /dev/null
    
