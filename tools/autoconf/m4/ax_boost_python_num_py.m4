# ===========================================================================
#       http://www.gnu.org/software/autoconf-archive/ax_epics4.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_BOOST_PYTHON_NUMPY()
#
# DESCRIPTION
#
#   Test for Boost.Python.NumPy libraries 
#
#   This macro sets:
#
#     HAVE_BOOST_PYTHON_NUMPY
#
# LICENSE
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 2

AC_DEFUN([AX_BOOST_PYTHON_NUMPY],

[AC_ARG_WITH([boost-python-numpy],
    [AS_HELP_STRING([--with-boost-python-numpy=BOOST_PYTHON_NUMPY_DIR],
        [Use BOOST_PYTHON_NUMPY from the specified location])
    ],
    [
        if test -z "$withval"; then 
            AC_MSG_ERROR(--with-boost-python-numpy requires directory name)
        else
            ac_boost_python_numpy_dir_path="$withval"
        fi
    ],
    [ac_boost_python_numpy_dir_path=""])

    if test -z "$ac_boost_python_numpy_dir_path"; then
        if test -z "$BOOST_PYTHON_NUMPY_DIR"; then
            ac_boost_python_numpy_dir_path=$BOOST_DIR
        else
            ac_boost_python_numpy_dir_path=$BOOST_PYTHON_NUMPY_DIR
        fi
    fi
    if ! test -d "$ac_boost_python_numpy_dir_path"; then
        AC_MSG_ERROR($ac_boost_python_numpy_dir_path is not a valid directory path)
    fi

    # save compile/link flags, set lang to C++
    AC_REQUIRE([AC_PROG_CXX])
    AC_LANG_PUSH([C++])
    SAVED_CPPFLAGS="$CPPFLAGS"
    SAVED_LDFLAGS="$LDFLAGS"

    # define BOOST_PYTHON_NUMPY_DIR
    BOOST_PYTHON_NUMPY_DIR=$ac_boost_python_numpy_dir_path

    # options for building with Boost.Python.NumPy
    BOOST_HOST_ARCH=$EPICS_HOST_ARCH
    if test -z "$BOOST_HOST_ARCH"; then
        BOOST_HOST_ARCH=`uname | tr [A-Z] [a-z]`-`uname -m`
    fi
    BOOST_PYTHON_NUMPY_CPPFLAGS=""
    BOOST_PYTHON_NUMPY_LDFLAGS=""
    if test "$BOOST_PYTHON_NUMPY_DIR" != "$BOOST_DIR"; then
        BOOST_PYTHON_NUMPY_CPPFLAGS="-I$BOOST_PYTHON_NUMPY_DIR/include"
        BOOST_PYTHON_NUMPY_LDFLAGS="-L$BOOST_PYTHON_NUMPY_DIR/lib -L$BOOST_PYTHON_NUMPY_DIR/lib/$BOOST_HOST_ARCH"
    fi

    # Use the same ending for boost_numpy as it it for boost_python
    BOOST_PYTHON_NUMPY_LIB="boost_numpy"`echo $BOOST_PYTHON_LIB | sed 's?boost_python??'`
    BOOST_PYTHON_NUMPY_LIBS="-l$BOOST_PYTHON_NUMPY_LIB -l$BOOST_PYTHON_LIB"

    export CPPFLAGS="$BOOST_CPPFLAGS $PYTHON_CPPFLAGS $BOOST_PYTHON_NUMPY_CPPFLAGS"
    export LDFLAGS="$BOOST_LDFLAGS $PYTHON_LDFLAGS $BOOST_PYTHON_NUMPY_LDFLAGS"

    # test basic libraries
    AC_MSG_CHECKING(for Boost.Python.NumPy library)
    export LIBS="$BOOST_PYTHON_NUMPY_LIBS"
    AC_LINK_IFELSE([AC_LANG_PROGRAM(
        [[
        #include "boost/python/numpy.hpp"
        ]],
        [[
        boost::python::numpy::initialize();
        ]])
    ],[succeeded=yes],[succeeded=no])

    if test "$succeeded" != "yes" ; then
        AC_MSG_RESULT([no])
        AC_SUBST(HAVE_BOOST_PYTHON_NUMPY,0)
        AC_SUBST(BOOST_PYTHON_NUMPY_DIR,'')
        AC_SUBST(BOOST_PYTHON_NUMPY_CPPFLAGS,'')
        AC_SUBST(BOOST_PYTHON_NUMPY_LDFLAGS,'')
        AC_SUBST(BOOST_PYTHON_NUMPY_LIBS,'')
    else
        AC_MSG_RESULT([yes])
        AC_SUBST(HAVE_BOOST_PYTHON_NUMPY,1)
        AC_SUBST(BOOST_PYTHON_NUMPY_DIR)
        AC_SUBST(BOOST_PYTHON_NUMPY_CPPFLAGS)
        AC_SUBST(BOOST_PYTHON_NUMPY_LDFLAGS)
        AC_SUBST(BOOST_PYTHON_NUMPY_LIBS,$BOOST_PYTHON_NUMPY_LIB)
    fi

    # restore compile/link flags
    CPPFLAGS="$SAVED_CPPFLAGS"
    LDFLAGS="$SAVED_LDFLAGS"
    AC_LANG_POP([C++])
])
