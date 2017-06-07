#!/bin/bash

MY_DIR=`dirname $0` && cd $MY_DIR && MY_DIR=`pwd`
ENV_FILE=$MY_DIR/build_env.sh
if [ ! -f $ENV_FILE ]; then
    echo "Cannot find $ENV_FILE."
exit 1
fi
source $ENV_FILE

echo Building boost-numpy $BOOST_NUMPY_VERSION
export PATH=$PYTHON_DIR/bin:$PATH
export LD_LIBRARY_PATH=$PYTHON_DIR/lib:$LD_LIBRARY_PATH:$BOOST_DIR/lib/$EPICS_HOST_ARCH
PYTHON_MAJOR_MINOR_VERSION=`python --version 2>&1 | cut -f2 -d ' ' | cut -f1,2 -d '.'`
export BOOST_ROOT=$BOOST_DIR
export BOOST_INCLUDEDIR=$BOOST_DIR/include
export BOOST_LIBRARYDIR=$BOOST_DIR/lib/$EPICS_HOST_ARCH

export PYTHON_INCLUDE_DIR=$PYTHON_DIR/include/python${PYTHON_MAJOR_MINOR_VERSION}
export PYTHON_LIBRARY=$PYTHON_DIR/lib/libpython${PYTHON_MAJOR_MINOR_VERSION}.so

BOOST_NUMPY_INSTALL_DIR=$BOOST_NUMPY_DIR
BOOST_NUMPY_TAR_FILE=$SRC_DIR/boost-numpy-$BOOST_NUMPY_VERSION.tar.gz
mkdir -p $BUILD_DIR
cd $BUILD_DIR
tar zxf $BOOST_NUMPY_TAR_FILE || exit 1
cd boost-numpy-$BOOST_NUMPY_VERSION || exit 1

mkdir -p build
cd build

cmake -D Boost_NO_BOOST_CMAKE=ON -D CMAKE_PREFIX_PATH=$BOOST_NUMPY_INSTALL_DIR -D CMAKE_INSTALL_PREFIX=$BOOST_NUMPY_INSTALL_DIR -D CMAKE_LIBRARY_OUTPUT_DIRECTORY=$BOOST_NUMPY_INSTALL_DIR/lib/$EPICS_HOST_ARCH -D PYTHON_LIBRARY=$PYTHON_LIBRARY .. || exit 1

make || exit 1
make install || exit 1

# Create link
cd $OPT_DIR && rm -f boost-numpy && ln -s `basename ${BOOST_NUMPY_INSTALL_DIR}` boost-numpy

