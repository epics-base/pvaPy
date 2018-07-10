#!/bin/sh

# Builds epics for pip

TOP=`dirname $0` && cd $TOP && TOP=`pwd`
BUILD_DIR=$TOP/build
EPICS_BUILD_DIR=$BUILD_DIR/epics
EPICS_BASE_DIR=$TOP/epics-base
DEPLOY_CONF=$TOP/../../../configure/DEPLOY.conf
if [ ! -f $DEPLOY_CONF ]; then
    echo "$DEPLOY_CONF not found"
    exit 1
fi
. $DEPLOY_CONF

EPICS_BASE_DOWNLOAD_URL=https://epics.anl.gov/download/base/base-${EPICS_BASE_VERSION}.tar.gz


mkdir -p $EPICS_BUILD_DIR
cd $EPICS_BUILD_DIR

# Download.
echo "Building epics base $EPICS_BASE_VERSION"
EPICS_BASE_TAR_FILE=`basename $EPICS_BASE_DOWNLOAD_URL`
if [ ! -f $EPICS_BASE_TAR_FILE ]; then
    wget $EPICS_BASE_DOWNLOAD_URL || exit 1
fi
tar zxf $EPICS_BASE_TAR_FILE


# Build
cd base-${EPICS_BASE_VERSION}
EPICS_HOST_ARCH=`./startup/EpicsHostArch`
eval "cat configure/CONFIG_SITE | sed 's?#INSTALL_LOCATION=.*?INSTALL_LOCATION=$EPICS_BASE_DIR?' > configure/CONFIG_SITE.2 && mv configure/CONFIG_SITE.2 configure/CONFIG_SITE"
echo "Using BUILD_FLAGS: $BUILD_FLAGS"
make $BUILD_FLAGS
make install 

echo "Copying startup files"
rsync -arvlP startup $EPICS_BASE_DIR/

# Fix rpath
cd $EPICS_BASE_DIR/lib/$EPICS_HOST_ARCH
newRpath="\$ORIGIN/."
for f in `find . -type f -name '*.so*'`; do
    echo "Checking RPATH for file: $f"
    chmod u+w $f
    rpath=`readelf -d $f 2> /dev/null | grep RPATH | awk '{print $NF}'`
    if [ ! -z "$rpath" ]; then
        echo "Replacing RPATH: $rpath => $newRpath"
        patchelf --set-rpath $newRpath $f || exit 1
    fi
done

cd $EPICS_BASE_DIR/bin/$EPICS_HOST_ARCH
newRpath="\$ORIGIN/../../lib/$EPICS_HOST_ARCH"
for f in `find . -type f`; do
    isElf=`file $f | grep ELF`
    chmod u+w $f
    if [ ! -z "$isElf" ]; then
        echo "Checking RPATH for file: $f"
        rpath=`readelf -d $f 2> /dev/null | grep RPATH | awk '{print $NF}'`
        if [ ! -z "$rpath" ]; then
            echo "Replacing RPATH: $rpath => $newRpath"
            patchelf --set-rpath $newRpath $f || exit 1
        fi
    fi
done

