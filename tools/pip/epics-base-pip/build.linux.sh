#!/bin/sh

# Builds epics for pip

TOP=`dirname $0` && cd $TOP && TOP=`pwd`
BUILD_DIR=$TOP/build
BUILD_SAVE_DIR=$TOP/../build
EPICS_BUILD_DIR=$BUILD_DIR/epics
EPICS_BASE_DIR=$TOP/epics-base
BUILD_CONF=$TOP/../../../configure/BUILD.conf
if [ ! -f $BUILD_CONF ]; then
    echo "$BUILD_CONF not found"
    exit 1
fi
. $BUILD_CONF

EPICS_BASE_DOWNLOAD_URL=https://epics.anl.gov/download/base/base-${EPICS_BASE_VERSION}.tar.gz


mkdir -p $BUILD_SAVE_DIR
mkdir -p $EPICS_BUILD_DIR
cd $EPICS_BUILD_DIR

# Download.
echo "Building epics base $EPICS_BASE_VERSION"
EPICS_BASE_TAR_FILE=`basename $EPICS_BASE_DOWNLOAD_URL`
if [ ! -f $EPICS_BASE_TAR_FILE ]; then
    curl $EPICS_BASE_DOWNLOAD_URL -o $EPICS_BASE_TAR_FILE || exit 1
fi
tar zxf $EPICS_BASE_TAR_FILE


# Build
cd base-*${EPICS_BASE_VERSION}
EPICS_HOST_ARCH=`./startup/EpicsHostArch`
CONFIG_FILE=configure/CONFIG_SITE
eval "cat $CONFIG_FILE | sed 's?#INSTALL_LOCATION=.*?INSTALL_LOCATION=$EPICS_BASE_DIR?' > $CONFIG_FILE.2 && mv $CONFIG_FILE.2 $CONFIG_FILE"
CONFIG_FILE=configure/os/CONFIG_SITE.Common.linux-x86_64
eval "cat $CONFIG_FILE | grep -v GNU_DIR | sed 's?COMMANDLINE_LIBRARY.*?COMMANDLINE_LIBRARY=EPICS?' > $CONFIG_FILE.2 && mv $CONFIG_FILE.2 $CONFIG_FILE"
CONFIG_FILE=configure/os/CONFIG_SITE.Common.linux-x86
eval "cat $CONFIG_FILE | grep -v GNU_DIR | sed 's?COMMANDLINE_LIBRARY.*?COMMANDLINE_LIBRARY=EPICS?' > $CONFIG_FILE.2 && mv $CONFIG_FILE.2 $CONFIG_FILE"
echo "Using BUILD_FLAGS: $BUILD_FLAGS"
make $BUILD_FLAGS
make install 

echo "Copying startup files"
rsync -arvlP startup $EPICS_BASE_DIR/

# Make sure permissions are correct for perl script
chmod a+x $EPICS_BASE_DIR/lib/perl/EpicsHostArch.pl

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

# Save build so we can reuse it
EPICS_BASE_SAVE_DIR=$BUILD_SAVE_DIR/epics-base-${EPICS_BASE_VERSION}
rsync -arlP $EPICS_BASE_DIR/ $EPICS_BASE_SAVE_DIR/
