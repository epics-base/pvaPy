// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PVAPY_ENVIRONMENT_H
#define PVAPY_ENVIRONMENT_H

#if defined HAVE_BOOST_PYTHON_NUMPY && HAVE_BOOST_PYTHON_NUMPY == 1

#define HAVE_NUMPY_SUPPORT 1
#define NUMPY_HEADER_FILE "boost/python/numpy.hpp"
namespace boost { namespace python { namespace numpy {} } }
namespace numpy_ = boost::python::numpy;

#else

#if defined HAVE_BOOST_NUMPY && HAVE_BOOST_NUMPY == 1
#define HAVE_NUMPY_SUPPORT 1
#define NUMPY_HEADER_FILE "boost/numpy.hpp"
namespace boost { namespace numpy {} }
namespace numpy_ = boost::numpy;
#endif // if defined HAVE_BOOST_NUMPY && HAVE_BOOST_NUMPY == 1

#endif // if defined HAVE_BOOST_PYTHON_NUMPY && HAVE_BOOST_PYTHON_NUMPY == 1

#endif // PVAPY_ENVIRONMENT_H

