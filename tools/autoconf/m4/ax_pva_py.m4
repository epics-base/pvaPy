# ===========================================================================
#       http://www.gnu.org/software/autoconf-archive/ax_epics_base.html
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

[
    # check for existing RELEASE.local file
    PVA_PY_TOP=../..
    current_dir=`pwd`
    cd $PVA_PY_TOP > /dev/null && PVA_PY_TOP=`pwd` && cd $current_dir > /dev/null
    release_local=$PVA_PY_TOP/configure/RELEASE.local
    AC_MSG_CHECKING(for existing pvaPy $release_local file)
    if test -f $release_local; then
        AC_MSG_RESULT([yes])
        AC_MSG_ERROR(you must remove existing $release_local file in order to recreate pvaPy configuration)
    else
        AC_MSG_RESULT([no])
    fi

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
    echo "EPICS4_DIR=$EPICS4_DIR" >> $release_local
    echo "EPICS_BASE=$EPICS_BASE" >> $release_local
    AC_MSG_NOTICE([created $release_local file])

    # create CONFIG_SITE.local
    echo "EPICS_HOST_ARCH=$EPICS_HOST_ARCH" >> $config_site_local
    echo "BOOST_PYTHON_LIB=$BOOST_PYTHON_LIB" >> $config_site_local
    echo "PVA_PY_CPPFLAGS=-DPVA_API_VERSION=$PVA_API_VERSION -DPVA_RPC_API_VERSION=$PVA_RPC_API_VERSION $BOOST_CPPFLAGS $PYTHON_CPPFLAGS" >> $config_site_local
    echo "PVA_PY_LDFLAGS=$BOOST_LDFLAGS $PYTHON_LDFLAGS" >> $config_site_local
    AC_MSG_NOTICE([created $config_site_local file])

    
    # create setup.sh
    setup_sh=$PVA_PY_TOP/setup.sh
    AC_MSG_CHECKING(for existing $setup_sh file)
    if test -f $setup_sh; then
        AC_MSG_RESULT([yes])
        AC_MSG_NOTICE(will not overwrite $setup_sh file)
    else
        AC_MSG_RESULT([no])
        cat >> $setup_sh << EOF
#!/bin/sh
# 
# pvaPy sh setup script
#
# modifies PYTHONPATH environment variable
#
if test -z "\$PYTHONPATH" ; then
    export PYTHONPATH=$PVA_PY_TOP/lib/$EPICS_HOST_ARCH
else
    export PYTHONPATH=$PVA_PY_TOP/lib/$EPICS_HOST_ARCH:\$PYTHONPATH
fi
EOF
        AC_MSG_NOTICE([created $setup_sh file])
    fi

    # create setup.csh
    setup_csh=$PVA_PY_TOP/setup.csh
    AC_MSG_CHECKING(for existing $setup_csh file)
    if test -f $setup_csh; then
        AC_MSG_RESULT([yes])
        AC_MSG_NOTICE(will not overwrite $setup_csh file)
    else
        AC_MSG_RESULT([no])
        cat >> $setup_csh << EOF
#!/bin/csh
# 
# pvaPy csh setup script
#
# modifies PYTHONPATH environment variable
#
if ( ! \$?PYTHONPATH ) then
    setenv PYTHONPATH ${PVA_PY_TOP}/lib/${EPICS_HOST_ARCH}
else
    setenv PYTHONPATH ${PVA_PY_TOP}/lib/${EPICS_HOST_ARCH}:\${PYTHONPATH}
endif
EOF
        AC_MSG_NOTICE([created $setup_csh file])
    fi
])
