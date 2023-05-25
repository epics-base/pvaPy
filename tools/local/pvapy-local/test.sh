#!/bin/bash

# Tests pvapy for local installation

TOP=`dirname $0` && cd $TOP && TOP=`pwd`
DEFAULT_PREFIX=$TOP/.. && cd $DEFAULT_PREFIX && DEFAULT_PREFIX=`pwd`
PREFIX=${PREFIX:-$DEFAULT_PREFIX}
BUILD_DIR=$TOP/build
BUILD_CONF=$TOP/../../../configure/BUILD.conf
if [ ! -f $BUILD_CONF ]; then
    echo "$BUILD_CONF not found"
    exit 1
fi
. $BUILD_CONF

if [ -z "$PVAPY_VERSION" ]; then
    PVAPY_VERSION=local
fi

OPT_EPICS_DIR=opt/epics-${EPICS_BASE_VERSION}
LOCAL_EPICS_DIR=$PREFIX/$OPT_EPICS_DIR
EPICS_HOST_ARCH=`$LOCAL_EPICS_DIR/startup/EpicsHostArch`

OPT_BOOST_DIR=opt/boost-python-${BOOST_VERSION}
LOCAL_BOOST_DIR=$PREFIX/$OPT_BOOST_DIR
BOOST_HOST_ARCH=`uname | tr [A-Z] [a-z]`-`uname -m`

OPT_PVAPY_DIR=opt/pvapy-${PVAPY_VERSION}
PVAPY_BUILD_DIR=$BUILD_DIR/pvaPy-$PVAPY_VERSION
LOCAL_PVAPY_DIR=$PREFIX/$OPT_PVAPY_DIR
LOCAL_PVAPY_DOC_DIR=$LOCAL_PVAPY_DIR/doc

# Download.
echo "Testing pvapy $PVAPY_VERSION in $LOCAL_PVAPY_DIR (PREFIX=$PREFIX}"
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Determine python configuration
PYTHON_BIN=`which python$PYTHON_VERSION 2> /dev/null`
if [ -z "$PYTHON_BIN" ]; then
    PYTHON_BIN=`which python 2> /dev/null`
fi
if [ -z "$PYTHON_BIN" ]; then
    echo "Python executable not found."
    exit 1
fi
PYTHON_DIR=`dirname \`dirname $PYTHON_BIN\``
export PATH=$PYTHON_DIR/bin:$PATH
export LD_LIBRARY_PATH=$PYTHON_DIR/lib:$LD_LIBRARY_PATH

PYTHON_MAJOR_MINOR_VERSION=`$PYTHON_BIN --version 2>&1 | cut -f2 -d ' ' | cut -f1,2 -d '.'`
PYTHON_MAJOR_VERSION=`echo $PYTHON_MAJOR_MINOR_VERSION | cut -f1 -d '.'`
PYTHON_LIB=`ls -c1 $PYTHON_DIR/lib/libpython${PYTHON_MAJOR_MINOR_VERSION}*.so.* 2> /dev/null` 
LOCAL_PVAPY_LIB_DIR=$LOCAL_PVAPY_DIR/lib/python$PYTHON_MAJOR_MINOR_VERSION/lib-dynload
LOCAL_PY_LIB_DIR=$PREFIX/lib/python$PYTHON_MAJOR_MINOR_VERSION/lib-dynload

echo "Sourcing setup file $PVAPY_BUILD_DIR/bin/$EPICS_HOST_ARCH/pvapy_setup_full.$PYTHON_MAJOR_MINOR_VERSION.sh"
source $PVAPY_BUILD_DIR/bin/$EPICS_HOST_ARCH/pvapy_setup_full.$PYTHON_MAJOR_MINOR_VERSION.sh

echo "Starting test server"
cd $PVAPY_BUILD_DIR/test/
./run_tests.sh
