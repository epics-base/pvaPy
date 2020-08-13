#!/bin/bash

# Builds pvapy for local installation

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
echo "Building pvapy $PVAPY_VERSION in $LOCAL_PVAPY_DIR (PREFIX=$PREFIX}"
mkdir -p $BUILD_DIR
cd $BUILD_DIR

PVAPY_TAR_FILE=pvaPy-$PVAPY_VERSION.tar.gz
if [ "$PVAPY_VERSION" = "local" ]; then
    echo "Creating $PVAPY_TAR_FILE from local sources"
    rm -rf pvaPy-local
    mkdir -p pvaPy-local
    SRC_TOP=$TOP/../../..
    rsync -arl $SRC_TOP/* --exclude "O.*" --exclude "build" pvaPy-local/
    tar zcvf $PVAPY_TAR_FILE pvaPy-local
fi

PVAPY_DOWNLOAD_URL=https://github.com/epics-base/pvaPy/archive/$PVAPY_VERSION.tar.gz
if [ ! -f $PVAPY_TAR_FILE ]; then
    echo "Downloading $PVAPY_TAR_FILE"
    curl -Ls -o $PVAPY_TAR_FILE -w %{url_effective} $PVAPY_DOWNLOAD_URL
    if [ $? -ne 0 ]; then
        PVAPY_GIT_URL=https://github.com/epics-base/pvaPy
        echo "$PVAPY_TAR_FILE does not exist, using git repository $PVAPY_GIT_URLi, branch $PVAPY_GIT_VERSION"
        if [ ! -d $PVAPY_BUILD_DIR ]; then
            git clone $PVAPY_GIT_URL $PVAPY_BUILD_DIR
        fi
        cd $PVAPY_BUILD_DIR
        git checkout $PVAPY_GIT_VERSION
    else
        tar zxf $PVAPY_TAR_FILE || exit 1
    fi
fi

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

PVAPY_FLAGS=""
if [ "$PYTHON_MAJOR_VERSION" = "3" ]; then
    PVAPY_FLAGS="PYTHON_VERSION=3"
fi
PVAPY_FLAGS="EPICS_BASE=$LOCAL_EPICS_DIR BOOST_ROOT=$LOCAL_BOOST_DIR PVAPY_ROOT=$LOCAL_PVAPY_DIR $PVAPY_FLAGS"
PVACCESS_BUILD_LIB_DIR=$PVAPY_BUILD_DIR/lib/python/$PYTHON_MAJOR_MINOR_VERSION/$EPICS_HOST_ARCH

PVACCESS_LIB_DIR=lib/python/$PYTHON_MAJOR_MINOR_VERSION/$EPICS_HOST_ARCH

echo "Using BUILD_FLAGS: $BUILD_FLAGS"
cd $PVAPY_BUILD_DIR
make configure $PVAPY_FLAGS || exit 1
make $BUILD_FLAGS || exit 1

echo "Building pvapy docs"
make doc || exit 1
mkdir -p $LOCAL_PVAPY_DOC_DIR
rsync -arvlP documentation/sphinx/_build/html $LOCAL_PVAPY_DOC_DIR/
rsync -arvlP README.md $LOCAL_PVAPY_DOC_DIR/

echo "Installing pvapy library"
mkdir -p $LOCAL_PVAPY_LIB_DIR
rsync -arv $PVACCESS_LIB_DIR/pvaccess.so $LOCAL_PVAPY_LIB_DIR/
mkdir -p $LOCAL_PY_LIB_DIR
rsync -arv $PVACCESS_LIB_DIR/pvaccess.so $LOCAL_PY_LIB_DIR/

