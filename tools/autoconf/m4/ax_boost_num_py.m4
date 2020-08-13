# ===========================================================================
#       http://www.gnu.org/software/autoconf-archive/ax_epics4.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_BOOST_NUMPY()
#
# DESCRIPTION
#
#   Test for Boost.NumPy libraries 
#
#   If no path to the installed Boost.NumPy directory is given, the macro 
#   evaluates $BOOST_NUMPY_DIR environment variable and searches under 
#   /usr/local directory
#
#   This macro sets:
#
#     HAVE_BOOST_NUMPY
#     BOOST_NUMPY_DIR
#     BOOST_NUMPY_CPPFLAGS
#     BOOST_NUMPY_LDFLAGS
#     BOOST_NUMPY_LIBS
#
# LICENSE
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 3

AC_DEFUN([AX_BOOST_NUMPY],

[AC_ARG_WITH([boost-numpy],
    [AS_HELP_STRING([--with-boost-numpy=BOOST_NUMPY_DIR],
        [Use BOOST_NUMPY from the specified location])
    ],
    [
        if test -z "$withval"; then 
            AC_MSG_ERROR(--with-boost-numpy requires directory name)
        else
            ac_boost_numpy_dir_path="$withval"
        fi
    ],
    [ac_boost_numpy_dir_path=""])

    if test -z "$ac_boost_numpy_dir_path"; then
        if test -z "$BOOST_NUMPY_DIR"; then
            ac_boost_numpy_dir_path=$BOOST_DIR
        else
            ac_boost_numpy_dir_path=$BOOST_NUMPY_DIR
        fi
    fi
    if ! test -d "$ac_boost_numpy_dir_path"; then
        AC_MSG_ERROR($ac_boost_numpy_dir_path is not a valid directory path)
    fi

    # save compile/link flags, set lang to C++
    AC_REQUIRE([AC_PROG_CXX])
    AC_LANG_PUSH([C++])
    SAVED_CPPFLAGS="$CPPFLAGS"
    SAVED_LDFLAGS="$LDFLAGS"

    # define BOOST_NUMPY_DIR
    BOOST_NUMPY_DIR=$ac_boost_numpy_dir_path

    # options for building with Boost.NumPy
    BOOST_HOST_ARCH=$EPICS_HOST_ARCH
    if test -z "$BOOST_HOST_ARCH"; then
        BOOST_HOST_ARCH=`uname | tr [A-Z] [a-z]`-`uname -m`
    fi
    BOOST_NUMPY_CPPFLAGS="-I$BOOST_NUMPY_DIR/include"
    BOOST_NUMPY_LDFLAGS="-L$BOOST_NUMPY_DIR/lib -L$BOOST_NUMPY_DIR/lib/$BOOST_HOST_ARCH"
    BOOST_NUMPY_LIBS="-lboost_numpy"

    export CPPFLAGS="$BOOST_CPPFLAGS $PYTHON_CPPFLAGS $BOOST_NUMPY_CPPFLAGS"
    export LDFLAGS="$BOOST_LDFLAGS $PYTHON_LDFLAGS $BOOST_NUMPY_LDFLAGS"

    # test basic libraries
    AC_MSG_CHECKING(for Boost.NumPy library)
    export LIBS="$BOOST_NUMPY_LIBS"
    AC_LINK_IFELSE([AC_LANG_PROGRAM(
        [[
        #include "boost/numpy.hpp"
        ]],
        [[
        boost::numpy::initialize();
        ]])
    ],[succeeded=yes],[succeeded=no])

    if test "$succeeded" != "yes" ; then
        if test "$HAVE_BOOST_PYTHON_NUMPY" = "1" ; then
            AC_MSG_RESULT([skipping, Boost.Python.NumPy already found])
            AC_SUBST(HAVE_BOOST_NUMPY,0)
            AC_SUBST(BOOST_NUMPY_DIR,'')
            AC_SUBST(BOOST_NUMPY_CPPFLAGS,'')
            AC_SUBST(BOOST_NUMPY_LDFLAGS,'')
            AC_SUBST(BOOST_NUMPY_LIBS,'')
        else
            AC_MSG_RESULT([no])
            AC_SUBST(HAVE_BOOST_NUMPY,0)
            AC_SUBST(BOOST_NUMPY_DIR,'')
            AC_SUBST(BOOST_NUMPY_CPPFLAGS,'')
            AC_SUBST(BOOST_NUMPY_LDFLAGS,'')
            AC_SUBST(BOOST_NUMPY_LIBS,'')
        fi
    else
        AC_MSG_RESULT([yes])
        AC_SUBST(HAVE_BOOST_NUMPY,1)
        AC_SUBST(BOOST_NUMPY_DIR)
        AC_SUBST(BOOST_NUMPY_CPPFLAGS)
        AC_SUBST(BOOST_NUMPY_LDFLAGS)
        AC_SUBST(BOOST_NUMPY_LIBS,boost_numpy)
    fi

    # restore compile/link flags
    CPPFLAGS="$SAVED_CPPFLAGS"
    LDFLAGS="$SAVED_LDFLAGS"
    AC_LANG_POP([C++])
])
