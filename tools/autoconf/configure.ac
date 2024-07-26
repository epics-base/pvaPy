AC_INIT([pvaPy], [5.4.0], [core-talk@aps.anl.gov])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_MACRO_DIR([m4])
AX_PYTHON_DEVEL([>=],[2.6])
AX_BOOST_BASE([1.78], [], [AC_MSG_ERROR(required Boost library version >= 1.78.)])
AX_BOOST_PYTHON
AX_BOOST_PYTHON_NUMPY
AX_BOOST_NUMPY
AX_EPICS_BASE([3.14.12])
AX_EPICS4([7.0.0])
AX_PVAPY([5.4.0])
#AC_OUTPUT

