// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#include "boost/python/class.hpp"
#include "boost/shared_ptr.hpp"

#include "ScalarArrayPyOwner.h"

using namespace boost::python;

//
// ScalarArrayPyOwner class
//
void wrapScalarArrayPyOwner()
{

class_<ScalarArrayPyOwner, boost::shared_ptr<ScalarArrayPyOwner> >("ScalarArrayPyOwner", "Class used to maintain ownership of scalar arrays.");

} // wrapScalarArrayPyOwner()

