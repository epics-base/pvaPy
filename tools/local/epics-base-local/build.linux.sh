#!/bin/bash

# Builds epics for local installation

TOP=`dirname $0` && cd $TOP && TOP=`pwd`
DEFAULT_PREFIX=$TOP/.. && cd $DEFAULT_PREFIX && DEFAULT_PREFIX=`pwd`
PREFIX=${PREFIX:-$DEFAULT_PREFIX}
BUILD_DIR=$TOP/build
EPICS_BUILD_DIR=$BUILD_DIR/epics
EPICS_EXECUTABLES="caget cainfo camonitor caput caRepeater p2p pvcall pvget pvinfo pvlist pvmonitor pvput softIoc softIocPVA"
BUILD_CONF=$TOP/../../../configure/BUILD.conf
if [ ! -f $BUILD_CONF ]; then
    echo "$BUILD_CONF not found"
    exit 1
fi
. $BUILD_CONF

OPT_EPICS_DIR=opt/epics-${EPICS_BASE_VERSION}
LOCAL_EPICS_DIR=$PREFIX/$OPT_EPICS_DIR
EPICS_BASE_DOWNLOAD_URL=https://epics.anl.gov/download/base/base-${EPICS_BASE_VERSION}.tar.gz

mkdir -p $EPICS_BUILD_DIR
cd $EPICS_BUILD_DIR

# Download.
echo "Building epics base $EPICS_BASE_VERSION in $LOCAL_EPICS_DIR (PREFIX=$PREFIX)"
EPICS_BASE_TAR_FILE=`basename $EPICS_BASE_DOWNLOAD_URL`
if [ ! -f $EPICS_BASE_TAR_FILE ]; then
    curl $EPICS_BASE_DOWNLOAD_URL -o $EPICS_BASE_TAR_FILE || exit 1
fi
tar zxf $EPICS_BASE_TAR_FILE

# Build
echo "Using BUILD_FLAGS: $BUILD_FLAGS"
cd base-*${EPICS_BASE_VERSION}
EPICS_HOST_ARCH=`./startup/EpicsHostArch`
echo "INSTALL_LOCATION=$LOCAL_EPICS_DIR" > configure/CONFIG_SITE.local

make $BUILD_FLAGS
make install 

echo "Copying startup files"
rsync -arvlP startup $LOCAL_EPICS_DIR/

echo "Creating executable symlinks"
TOP_DIR=..
mkdir -p $PREFIX/bin/$EPICS_HOST_ARCH
cd $PREFIX/bin/$EPICS_HOST_ARCH
for f in $EPICS_EXECUTABLES; do
    echo "Linking executable file: $f"
    rm -f $f && ln -s $LOCAL_EPICS_DIR/bin/$EPICS_HOST_ARCH/$f .
done

echo "Creating library symlinks"
mkdir -p $PREFIX/lib/$EPICS_HOST_ARCH
cd $PREFIX/lib/$EPICS_HOST_ARCH
EPICS_LIBRARIES=`find $LOCAL_EPICS_DIR/lib/$EPICS_HOST_ARCH -name '*.so*' -o -name '*.dylib*'`
for f in $EPICS_LIBRARIES; do
    libFile=`basename $f`
    echo "Linking library file: $libFile"
    rm -f $libFile && ln -s $LOCAL_EPICS_DIR/lib/$EPICS_HOST_ARCH/$libFile .
done

