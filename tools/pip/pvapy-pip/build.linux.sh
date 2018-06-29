#!/bin/sh

# Builds pvapy for pip


CURRENT_DIR=`pwd`
TOP_DIR=`dirname $0` && cd $TOP_DIR && TOP_DIR=`pwd`
DEPLOY_CONF=$TOP_DIR/../../../configure/DEPLOY.conf

if [ ! -f $DEPLOY_CONF ]; then
    echo "$DEPLOY_CONF not found"
    exit 1
fi
. $DEPLOY_CONF

DEFAULT_PYTHON_VERSION="2"
if [ -z "$PYTHON_VERSION" ]; then
    PYTHON_VERSION=$DEFAULT_PYTHON_VERSION
fi

BUILD_DIR=$TOP_DIR/build
PVA_PY_DIR=$TOP_DIR/pvapy
PVACCESS_DIR=$TOP_DIR/pvaccess
PVACCESS_DOC_DIR=$PVACCESS_DIR/doc
PVACCESS_LIB_DIR=$PVACCESS_DIR/lib
PVA_PY_BUILD_DIR=$BUILD_DIR/pvaPy-$PVA_PY_VERSION
EPICS_BASE_DIR=$TOP_DIR/../epics-base-pip/epics-base
EPICS_HOST_ARCH=`$EPICS_BASE_DIR/startup/EpicsHostArch`
BOOST_DIR=$TOP_DIR/../pvapy-boost-pip/pvapy-boost
BOOST_HOST_ARCH=`uname | tr [A-Z] [a-z]`-`uname -m`
PVACCESS_LIB=pvaccess.so

mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Download.
echo "Building pvapy $PVA_PY_VERSION"
PVA_PY_TAR_FILE=pvaPy-$PVA_PY_VERSION.tar.gz
PVA_PY_DOWNLOAD_URL=https://github.com/epics-base/pvaPy/archive/$PVA_PY_VERSION.tar.gz
if [ ! -f $PVA_PY_TAR_FILE ]; then
    echo "Downloading $PVA_PY_TAR_FILE"
    wget -q $PVA_PY_DOWNLOAD_URL && mv $PVA_PY_VERSION.tar.gz $PVA_PY_TAR_FILE 
    if [ $? -ne 0 ]; then
        PVA_PY_GIT_URL=https://github.com/epics-base/pvaPy
        echo "$PVA_PY_TAR_FILE does not exist, using git repository $PVA_PY_GIT_URLi, branch $PVA_PY_GIT_VERSION"
        if [ ! -d $PVA_PY_BUILD_DIR ]; then
            git clone $PVA_PY_GIT_URL $PVA_PY_BUILD_DIR
        fi
        cd $PVA_PY_BUILD_DIR
        git checkout $PVA_PY_GIT_VERSION
    else
        tar zxf $PVA_PY_TAR_FILE || exit 1
    fi
fi

PYTHON_BIN=`which python$PYTHON_VERSION 2> /dev/null`
if [ -z "$PYTHON_BIN" ]; then
    PYTHON_BIN=`which python 2> /dev/null`
fi
if [ ! -z "$PYTHON_DIR" ]; then
    if [ -f $PYTHON_DIR/bin/python$PYTHON_VERSION ]; then
        PYTHON_BIN=$PYTHON_DIR/bin/python$PYTHON_VERSION
    else
        PYTHON_BIN=$PYTHON_DIR/bin/python
    fi
else
    if [ -z "$PYTHON_BIN" ]; then
        echo "Python executable not found."
        exit 1
    fi
    PYTHON_DIR=`dirname \`dirname $PYTHON_BIN\``
fi
export PATH=$PYTHON_DIR/bin:$PATH
export LD_LIBRARY_PATH=$PYTHON_DIR/lib:$LD_LIBRARY_PATH:$BOOST_DIR/lib/$EPICS_HOST_ARCH:$PVA_PY_DIR/lib/$EPICS_HOST_ARCH

PYTHON_MAJOR_MINOR_VERSION=`$PYTHON_BIN --version 2>&1 | cut -f2 -d ' ' | cut -f1,2 -d '.'`
PYTHON_MAJOR_VERSION=`echo $PYTHON_MAJOR_MINOR_VERSION | cut -f1 -d '.'`
PVA_PY_FLAGS=""
if [ "$PYTHON_MAJOR_VERSION" = "3" ]; then
    PVA_PY_FLAGS="PYTHON_VERSION=3"
fi
PVA_PY_FLAGS="EPICS_BASE=$EPICS_BASE_DIR BOOST_ROOT=$BOOST_DIR PVA_PY_ROOT=$PVA_PY_DIR $PVA_PY_FLAGS"
PVACCESS_BUILD_LIB_DIR=$PVA_PY_BUILD_DIR/lib/python/$PYTHON_MAJOR_MINOR_VERSION/$EPICS_HOST_ARCH

echo "Building pvapy"
cd $PVA_PY_BUILD_DIR
make configure $PVA_PY_FLAGS || exit 1
make -j || exit 1

echo "Building pvapy docs"
make doc || exit 1
mkdir -p $PVACCESS_DOC_DIR
mkdir -p $PVA_PY_DIR
rsync -arvl documentation/sphinx/_build/html $PVACCESS_DOC_DIR/

echo "Installing pvapy library"
rsync -arv $PVACCESS_BUILD_LIB_DIR/$PVACCESS_LIB $PVACCESS_DIR/

echo "Generating python module init files"
echo "from pvaccess import *" > $PVACCESS_DIR/__init__.py
echo "from pvaccess import *" > $PVA_PY_DIR/__init__.py

echo "Copying dependencies"
EPICS_LIBS=`ls -c1 $EPICS_BASE_DIR/lib/$EPICS_HOST_ARCH/*.so`
BOOST_LIBS=`ls -c1 $BOOST_DIR/lib/$BOOST_HOST_ARCH/*.so`
mkdir -p $PVACCESS_LIB_DIR/$EPICS_HOST_ARCH
for lib in $EPICS_LIBS $BOOST_LIBS; do
    rsync -arvl ${lib}* $PVACCESS_LIB_DIR/$EPICS_HOST_ARCH/
done

# Fix rpath
cd $PVACCESS_DIR
newRpath="\$ORIGIN/lib/$EPICS_HOST_ARCH"
echo "Setting RPATH for file $PVACCESS_LIB to $newRpath"
chmod u+w $PVACCESS_LIB
patchelf --set-rpath $newRpath $PVACCESS_LIB  || exit 1

# Done
cd $CURRENT_DIR



