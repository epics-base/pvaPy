// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PVAPY_ENVIRONMENT_H
#define PVAPY_ENVIRONMENT_H

#if defined HAVE_BOOST_NUM_PY && HAVE_BOOST_NUM_PY == 1
#define HAVE_NUM_PY_SUPPORT 1
#define NUM_PY_HEADER_FILE "boost/numpy.hpp"
namespace boost { namespace numpy {} }
namespace numpy_ = boost::numpy;
#endif // if defined HAVE_BOOST_NUM_PY && HAVE_BOOST_NUM_PY == 1

#if defined HAVE_BOOST_PYTHON_NUM_PY && HAVE_BOOST_PYTHON_NUM_PY == 1
#define HAVE_NUM_PY_SUPPORT 1
#define NUM_PY_HEADER_FILE "boost/python/numpy.hpp"
namespace boost { namespace python { namespace numpy {} } }
namespace numpy_ = boost::python::numpy;
#endif // if defined HAVE_BOOST_PYTHON_NUM_PY && HAVE_BOOST_PYTHON_NUM_PY == 1

#endif // PVAPY_ENVIRONMENT_H

