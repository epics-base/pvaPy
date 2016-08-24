# ===========================================================================
#       http://www.gnu.org/software/autoconf-archive/ax_epics4.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_BOOST_NUM_PY()
#
# DESCRIPTION
#
#   Test for Boost.NumPy libraries 
#
#   If no path to the installed Boost.NumPy directory is given, the macro 
#   evaluates $BOOST_NUM_PY_DIR environment variable and searches under 
#   /usr/local directory
#
#   This macro sets:
#
#     HAVE_BOOST_NUM_PY
#     BOOST_NUM_PY_DIR
#     BOOST_NUM_PY_CPPFLAGS
#     BOOST_NUM_PY_LDFLAGS
#     BOOST_NUM_PY_LIBS
#
# LICENSE
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 1

AC_DEFUN([AX_BOOST_NUM_PY],

[AC_ARG_WITH([boost-numpy],
    [AS_HELP_STRING([--with-boost-numpy=BOOST_NUM_PY_DIR],
        [Use BOOST_NUM_PY from the specified location])
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
        if test -z "$BOOST_NUM_PY_DIR"; then
            ac_boost_numpy_dir_path="/usr/local"
        else
            ac_boost_numpy_dir_path=$BOOST_NUM_PY_DIR
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

    # define BOOST_NUM_PY_DIR
    BOOST_NUM_PY_DIR=$ac_boost_numpy_dir_path

    # options for building with Boost.NumPy
    BOOST_NUM_PY_CPPFLAGS="-I$BOOST_NUM_PY_DIR/include"
    BOOST_NUM_PY_LDFLAGS="-L$BOOST_NUM_PY_DIR/lib"
    BOOST_NUM_PY_LIBS="-lboost_numpy"

    export CPPFLAGS="$BOOST_CPPFLAGS $PYTHON_CPPFLAGS $BOOST_NUM_PY_CPPFLAGS"
    export LDFLAGS="$BOOST_LDFLAGS $PYTHON_LDFLAGS $BOOST_NUM_PY_LDFLAGS"

    # test basic libraries
    AC_MSG_CHECKING(for Boost.NumPy library)
    export LIBS="$BOOST_NUM_PY_LIBS"
    AC_LINK_IFELSE([AC_LANG_PROGRAM(
        [[
        #include "boost/numpy.hpp"
        ]],
        [[
        boost::numpy::initialize();
        ]])
    ],[succeeded=yes],[succeeded=no])

    if test "$succeeded" != "yes" ; then
        AC_MSG_RESULT([no])
        AC_SUBST(HAVE_BOOST_NUM_PY,0)
        AC_SUBST(BOOST_NUM_PY_DIR,'')
        AC_SUBST(BOOST_NUM_PY_CPPFLAGS,'')
        AC_SUBST(BOOST_NUM_PY_LDFLAGS,'')
        AC_SUBST(BOOST_NUM_PY_LIBS,'')
    else
        AC_MSG_RESULT([yes])
        AC_SUBST(HAVE_BOOST_NUM_PY,1)
        AC_SUBST(BOOST_NUM_PY_DIR)
        AC_SUBST(BOOST_NUM_PY_CPPFLAGS)
        AC_SUBST(BOOST_NUM_PY_LDFLAGS)
        AC_SUBST(BOOST_NUM_PY_LIBS,boost_numpy)
    fi

    # restore compile/link flags
    CPPFLAGS="$SAVED_CPPFLAGS"
    LDFLAGS="$SAVED_LDFLAGS"
    AC_LANG_POP([C++])
])
