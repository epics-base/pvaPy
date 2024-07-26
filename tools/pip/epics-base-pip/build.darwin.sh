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
cd base-${EPICS_BASE_VERSION}
EPICS_HOST_ARCH=`./startup/EpicsHostArch`
PVAPY_USE_CPP11=${PVAPY_USE_CPP11:-0}

eval "cat configure/CONFIG_SITE | sed 's?#INSTALL_LOCATION=.*?INSTALL_LOCATION=$EPICS_BASE_DIR?' > configure/CONFIG_SITE.2 && mv configure/CONFIG_SITE.2 configure/CONFIG_SITE"

for arch in 'darwin-x86' 'darwin-aarch64'; do
    CONFIG_FILE=configure/os/CONFIG_SITE.Common.$arch
    if [ $PVAPY_USE_CPP11 -gt 0 ]; then
        echo "OP_SYS_CXXFLAGS += -std=c++11" >> $CONFIG_FILE
    fi
done

echo "Using BUILD_FLAGS: $BUILD_FLAGS"
echo "Using C++11: $PVAPY_USE_CPP11"

make $BUILD_FLAGS
make install 

echo "Copying startup files"
rsync -arvlP startup $EPICS_BASE_DIR/

# Fix libraries and binaries
cd $EPICS_BASE_DIR/lib/$EPICS_HOST_ARCH
for lib in `find . -type f -name '*.dylib'`; do 
    cd $EPICS_BASE_DIR/lib/$EPICS_HOST_ARCH
    f=`basename $lib`
    echo "Checking file: $f (pwd: $PWD)"
    chmod u+w $f
    oldId=`otool -D $f | tail -1`
    newId="@loader_path/../../lib/$EPICS_HOST_ARCH/$f"
    echo "Modifying id $oldId => $newId"
    install_name_tool -id "$newId" $f
    for f2 in `find . -type f -name '*.dylib' | grep -v $f`; do 
        chmod u+w $f2
        install_name_tool -change "$oldId" "$newId" $f2
    done

    cd $EPICS_BASE_DIR/bin/$EPICS_HOST_ARCH
    for f2 in `find . -type f`; do
        isMach=`file $f2 | grep Mach-O`
        chmod u+w $f2
        if [ ! -z "$isMach" ]; then
            install_name_tool -change "$oldId" "$newId" $f2
        fi
    done
done

# Save build so we can reuse it
EPICS_BASE_SAVE_DIR=$BUILD_SAVE_DIR/epics-base-${EPICS_BASE_VERSION}
rsync -arlP $EPICS_BASE_DIR/ $EPICS_BASE_SAVE_DIR/

