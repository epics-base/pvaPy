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
    curl $EPICS_BASE_DOWNLOAD_URL -o $EPICS_BASE_TAR_FILE || exit 1
fi
tar zxf $EPICS_BASE_TAR_FILE


# Build
cd base-${EPICS_BASE_VERSION}
EPICS_HOST_ARCH=`./startup/EpicsHostArch`
eval "cat configure/CONFIG_SITE | sed 's?#INSTALL_LOCATION=.*?INSTALL_LOCATION=$EPICS_BASE_DIR?' > configure/CONFIG_SITE.2 && mv configure/CONFIG_SITE.2 configure/CONFIG_SITE"
make -j
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


