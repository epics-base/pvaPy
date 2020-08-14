#!/bin/sh

# This file contains common functionality and environment setup for all
# build scripts

#
# Software versions
#

EPICS_BASE_VERSION=3.15.4
EPICS4_VERSION=4.5.0
CROSS_COMPILER_TARGET_ARCHS="vxWorks-ppc32 vxWorks-ppc32-debug linux-arm"
VXWORKS_VERSION=6.9
ZYNQ_VERSION=2016.1
XILINX_QEMU_VERSION=2016.2

SDDS_VERSION=3.4
SDDS_EPICS_VERSION=3.4
EPICS_EXTENSIONS_VERSION=20120904
MEDM_VERSION=3_1_9
MSI_VERSION=1-7
#MCA_VERSION=R4-4
MCA_VERSION=4-3
MATLAB_VERSION=R2015a

TCL_VERSION=8.6.6
TK_VERSION=8.6.6
PYTHON_VERSION=2.7.13
PYTHON3_VERSION=3.6.3 
WX_PYTHON_VERSION=2.8.12.1
#WX_PYTHON_VERSION=3.0.2.0
QPID_VERSION=0.22
OCTAVE_VERSION=4.0.3
JAVA_VERSION=7u21
ANT_VERSION=1.9.1
CSS_VERSION=3.1.2

ZLIB_VERSION=1.2.8
XZ_VERSION=5.2.2
LIBGCRYPT_VERSION=1.7.2
LIBGPG_ERROR_VERSION=1.24
BOOST_VERSION=1.65.1
BOOST_PYTHON3_VERSION=1.65.1
BOOST_NUMPY_VERSION=2016.12.19
PVAPY_VERSION=0.9.1
PVAPY_PYTHON3_VERSION=0.9.1

ZEROMQ_VERSION=4.2.1
CZMQ_VERSION=4.0.2
JSONCPP_VERSION=1.7.7

PYQT_VERSION=4.12
SIP_VERSION=4.19.1

#
# EPICS modules and versions
#

SYNAPPS_GITHUB_URL=https://github.com/epics-modules
AD_GITHUB_URL=https://github.com/areaDetector

LOCAL_MODULE_LIST="ipac:2.14 seq:2.2.3 apsEvent:4.4 runcontrol:2.0.6 devlib2:2.7 asd3:1-1"
SYNAPPS_GITHUB_MODULE_LIST="asyn:R4-29 autosave:R5-7-1 busy:R1-6-1 sscan:R2-10-2 calc:R3-6-1 mrfioc2:2.0.4"
AD_GITHUB_MODULE_LIST="ADCore:R2-5"
#SYNAPPS_GITHUB_MODULE_LIST="asyn:R4-31 autosave:R5-7-1 busy:R1-6-1 sscan:R2-10-2 calc:R3-6-1 mrfioc2:2.0.4"
#AD_GITHUB_MODULE_LIST="ADCore:R2-6"

#
# Software directories
#

# This file will get sourced with MY_DIR already set
CURRENT_DIR=`pwd`
cd $MY_DIR/.. && TOP_DIR=`pwd`
SRC_DIR=$TOP_DIR/src
OPT_DIR=$TOP_DIR/opt
BUILD_DIR=$TOP_DIR/build
PATCHES_DIR=$TOP_DIR/patches
EPICS_DIR=$TOP_DIR/epics
EPICS_BASE_DIR=$EPICS_DIR/base-${EPICS_BASE_VERSION}
EPICS_EXTENSIONS_DIR=$EPICS_DIR/extensions
EPICS_MODULES_DIR=$EPICS_DIR/modules
EPICS4_DIR=$EPICS_MODULES_DIR/epics4-cpp-`echo $EPICS4_VERSION | sed 's?\.?-?g'`

export EPICS_HOST_ARCH=`uname | tr [A-Z] [a-z]`-`uname -m`
export EPICS_BASE=$EPICS_BASE_DIR
export EPICS_EXTENSIONS=$EPICS_EXTENSIONS_DIR

PYTHON_DIR=$OPT_DIR/python-$PYTHON_VERSION/$EPICS_HOST_ARCH
PYTHON3_DIR=$OPT_DIR/python3-$PYTHON3_VERSION/$EPICS_HOST_ARCH
#PYTHON_DIR=$OPT_DIR/python-$PYTHON_VERSION
TCL_DIR=$OPT_DIR/tcl-$TCL_VERSION/$EPICS_HOST_ARCH
TK_DIR=$OPT_DIR/tk-$TK_VERSION/$EPICS_HOST_ARCH
JAVA_DIR=$OPT_DIR/java-$JAVA_VERSION/$EPICS_HOST_ARCH
QPID_DIR=$OPT_DIR/qpid-$QPID_VERSION/$EPICS_HOST_ARCH
OCTAVE_DIR=$OPT_DIR/octave-$OCTAVE_VERSION/$EPICS_HOST_ARCH
ANT_DIR=$OPT_DIR/ant-$ANT_VERSION
CSS_DIR=$OPT_DIR/css-$CSS_VERSION

ZYNQ_DIR=/usr/local/vw/zynq-$ZYNQ_VERSION

XILINX_QEMU_DIR=$OPT_DIR/xilinx-qemu-$XILINX_QEMU_VERSION
ZLIB_DIR=$OPT_DIR/zlib-$ZLIB_VERSION
XZ_DIR=$OPT_DIR/xz-$XZ_VERSION
LIBGCRYPT_DIR=$OPT_DIR/libgcrypt-$LIBGCRYPT_VERSION
LIBGPG_ERROR_DIR=$OPT_DIR/libgpg-error-$LIBGPG_ERROR_VERSION
BOOST_DIR=$OPT_DIR/boost-$BOOST_VERSION
BOOST_PYTHON3_DIR=$OPT_DIR/boost-python3-$BOOST_PYTHON3_VERSION
BOOST_NUMPY_DIR=$OPT_DIR/boost-numpy-$BOOST_NUMPY_VERSION
PVAPY_DIR=$EPICS_MODULES_DIR/pvaPy-$PVAPY_VERSION
PVAPY_PYTHON3_DIR=$EPICS_MODULES_DIR/pvaPy-python3-$PVAPY_PYTHON3_VERSION

ZEROMQ_DIR=$OPT_DIR/zeromq-$ZEROMQ_VERSION
CZMQ_DIR=$ZEROMQ_DIR
JSONCPP_DIR=$OPT_DIR/jsoncpp-$JSONCPP_VERSION

#
# Common build functions
#

# Apply patches.
applyPatches() {
    dir=$1
    cd $PATCHES_DIR
    patchList=`find . -name '*.patch' | grep "$dir"`
    addList=`find . -name '*.add' | grep "$dir"`
    cd $TOP_DIR
    if [ ! -z "$addList" ]; then
        echo "Applying additions: $addList"
        for f in $addList; do
            targetFile=`echo $f | sed 's?\.add??'`
            rsync -arlP --exclude '*.svn' $PATCHES_DIR/$f $targetFile
            echo "Adding target: $targetFile"
        done
    fi
    if [ ! -z "$patchList" ]; then
        echo "Applying patches: $patchList"
        for f in $patchList; do
            targetFile=`echo $f | sed 's?\.patch??'`
            rsync -arlP --exclude '*.svn' $PATCHES_DIR/$f $f
	    patch -N --dry-run --silent $targetFile $f 2>/dev/null
	    if [ $? -eq 0 ]; then
	        # Apply the patch
                echo "Patching target: $targetFile"
		patch -N $targetFile $f
            else
                echo "Target $targetFile has already been patched"
	    fi
        done
    fi
}

# Configure cross compile
configureCrossCompile() {
    _file=$1
    if [ ! -f $_file.orig ]; then
        cp $_file $_file.orig
    fi
    eval "cat $_file | sed 's?#CROSS_COMPILER_TARGET_ARCHS=.*?CROSS_COMPILER_TARGET_ARCHS=?' | sed 's?CROSS_COMPILER_TARGET_ARCHS.*=.*?CROSS_COMPILER_TARGET_ARCHS=${CROSS_COMPILER_TARGET_ARCHS}?' > $_file.tmp && cp $_file.tmp $_file"
}

# Disable cross compile
disableCrossCompile() {
    _file=$1
    eval "cat $_file | sed '/\.*CROSS_COMPILER_TARGET_ARCHS\.*/CROSS_COMPILER_TARGET_ARCHS =' > $_file.tmp && mv $_file.tmp $_file"
}

# Configure epics base
configureEpicsBase() {
    _file=$1
    eval "cat $_file | sed 's?EPICS_BASE=.*?EPICS_BASE=${EPICS_BASE_DIR}?' > $_file.tmp && mv $_file.tmp $_file"
}

# Execute command
execute() {
    echo "Executing: $@"
    eval "$@"
}

