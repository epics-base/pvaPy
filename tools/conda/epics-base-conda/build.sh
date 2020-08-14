#!/bin/sh

# Builds epics for conda

OPT_EPICS_DIR=opt/epics
CONDA_EPICS_DIR=$PREFIX/$OPT_EPICS_DIR
EPICS_HOST_ARCH=`./startup/EpicsHostArch`
EPICS_EXECUTABLES="caget cainfo camonitor caput caRepeater makeBaseApp.pl p2p pvcall pvget pvinfo pvlist pvmonitor pvput softIoc softIocPVA"

echo "Building epics"
#eval "cat configure/CONFIG_SITE | sed 's?#INSTALL_LOCATION=.*?INSTALL_LOCATION=$CONDA_EPICS_DIR?' > configure/CONFIG_SITE.2 && mv configure/CONFIG_SITE.2 configure/CONFIG_SITE" 
echo "INSTALL_LOCATION=$CONDA_EPICS_DIR" > configure/CONFIG_SITE.local

CONFIG_FILE=configure/os/CONFIG_SITE.Common.linux-x86_64
eval "cat $CONFIG_FILE | grep -v GNU_DIR | sed 's?COMMANDLINE_LIBRARY.*?COMMANDLINE_LIBRARY=EPICS?' > $CONFIG_FILE.2 && mv $CONFIG_FILE.2 $CONFIG_FILE"
CONFIG_FILE=configure/os/CONFIG_SITE.Common.linux-x86
eval "cat $CONFIG_FILE | grep -v GNU_DIR | sed 's?COMMANDLINE_LIBRARY.*?COMMANDLINE_LIBRARY=EPICS?' > $CONFIG_FILE.2 && mv $CONFIG_FILE.2 $CONFIG_FILE"


echo "Using BUILD_FLAGS: $BUILD_FLAGS"
make $BUILD_FLAGS
make install 

echo "Copying startup files"
rsync -arvlP startup $CONDA_EPICS_DIR/

echo "Creating executable symlinks"
TOP_DIR=..
cd $PREFIX/bin
for f in $EPICS_EXECUTABLES; do
    echo "Linking executable file: $f"
    ln -s $TOP_DIR/$OPT_EPICS_DIR/bin/$EPICS_HOST_ARCH/$f .
done

echo "Creating library symlinks"
cd $PREFIX/lib
EPICS_LIBRARIES=`find $CONDA_EPICS_DIR/lib/$EPICS_HOST_ARCH -name '*.so*' -o -name '*.dylib*'`
for f in $EPICS_LIBRARIES; do
    libFile=`basename $f`
    echo "Linking library file: $libFile"
    ln -s $TOP_DIR/$OPT_EPICS_DIR/lib/$EPICS_HOST_ARCH/$libFile .
done

