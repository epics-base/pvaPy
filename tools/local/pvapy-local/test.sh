#!/bin/bash

# Tests pvapy for local installation

TOP=`dirname $0` && cd $TOP && TOP=`pwd`
DEFAULT_PREFIX=$TOP/.. && cd $DEFAULT_PREFIX && DEFAULT_PREFIX=`pwd`
PREFIX=${PREFIX:-$DEFAULT_PREFIX}
BUILD_DIR=$TOP/build
DEPLOY_CONF=$TOP/../../../configure/DEPLOY.conf
if [ ! -f $DEPLOY_CONF ]; then
    echo "$DEPLOY_CONF not found"
    exit 1
fi
. $DEPLOY_CONF

if [ -z "$PVA_PY_VERSION" ]; then
    PVA_PY_VERSION=local
fi

OPT_EPICS_DIR=opt/epics-${EPICS_BASE_VERSION}
LOCAL_EPICS_DIR=$PREFIX/$OPT_EPICS_DIR
EPICS_HOST_ARCH=`$LOCAL_EPICS_DIR/startup/EpicsHostArch`

OPT_BOOST_DIR=opt/boost-python-${BOOST_VERSION}
LOCAL_BOOST_DIR=$PREFIX/$OPT_BOOST_DIR
BOOST_HOST_ARCH=`uname | tr [A-Z] [a-z]`-`uname -m`

OPT_PVA_PY_DIR=opt/pvapy-${PVA_PY_VERSION}
PVA_PY_BUILD_DIR=$BUILD_DIR/pvaPy-$PVA_PY_VERSION
LOCAL_PVA_PY_DIR=$PREFIX/$OPT_PVA_PY_DIR
LOCAL_PVA_PY_DOC_DIR=$LOCAL_PVA_PY_DIR/doc

# Download.
echo "Testing pvapy $PVA_PY_VERSION in $LOCAL_PVA_PY_DIR (PREFIX=$PREFIX}"
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
LOCAL_PVA_PY_LIB_DIR=$LOCAL_PVA_PY_DIR/lib/python$PYTHON_MAJOR_MINOR_VERSION/lib-dynload
LOCAL_PY_LIB_DIR=$PREFIX/lib/python$PYTHON_MAJOR_MINOR_VERSION/lib-dynload

echo "Sourcing setup file $PVA_PY_BUILD_DIR/bin/$EPICS_HOST_ARCH/pvapy_setup_full.$PYTHON_MAJOR_MINOR_VERSION.sh"
source $PVA_PY_BUILD_DIR/bin/$EPICS_HOST_ARCH/pvapy_setup_full.$PYTHON_MAJOR_MINOR_VERSION.sh

echo "Starting test server"
cd $PVA_PY_BUILD_DIR/test/
python testServer.py 10  &

echo "Starting tests"
nosetests -v testPvObject.py
nosetests -v testChannelPut.py

wait 
echo "Tests done"

