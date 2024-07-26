#!/bin/bash

# Builds pvapy-boost for local installation

TOP=`dirname $0` && cd $TOP && TOP=`pwd`
DEFAULT_PREFIX=$TOP/.. && cd $DEFAULT_PREFIX && DEFAULT_PREFIX=`pwd`
PREFIX=${PREFIX:-$DEFAULT_PREFIX}
BUILD_DIR=$TOP/build
BOOST_HOST_ARCH=`uname | tr [A-Z] [a-z]`-`uname -m`
BUILD_CONF=$TOP/../../../configure/BUILD.conf
if [ ! -f $BUILD_CONF ]; then
    echo "$BUILD_CONF not found"
    exit 1
fi
. $BUILD_CONF

OPT_BOOST_DIR=boost-python-${BOOST_VERSION}
LOCAL_BOOST_DIR=$PREFIX/$OPT_BOOST_DIR
BOOST_DOWNLOAD_VERSION=`echo ${BOOST_VERSION} | sed 's?\.?_?g'`
BOOST_DOWNLOAD_URL=https://sourceforge.net/projects/boost/files/boost/$BOOST_VERSION/boost_${BOOST_DOWNLOAD_VERSION}.tar.gz

mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Download.
echo "Building boost python $BOOST_VERSION in $LOCAL_BOOST_DIR (PREFIX=$PREFIX)"
BOOST_TAR_FILE=`basename $BOOST_DOWNLOAD_URL`
if [ ! -f $BOOST_TAR_FILE ]; then
    curl -Ls -o $BOOST_TAR_FILE -w %{url_effective} $BOOST_DOWNLOAD_URL > /dev/null || exit 1
fi
BOOST_BUILD_DIR=`echo $BOOST_TAR_FILE | sed 's?.tar.gz??g'`

rm -rf $BOOST_BUILD_DIR
tar xf $BOOST_TAR_FILE
cd $BOOST_BUILD_DIR

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
fi
if [ -z "$PYTHON_BIN" ]; then
    echo "Python executable not found."
    exit 1
fi

PYTHON_DIR=`dirname \`dirname $PYTHON_BIN\``
export PATH=$PYTHON_DIR/bin:$PATH
export LD_LIBRARY_PATH=$PYTHON_DIR/lib:$LD_LIBRARY_PATH:$BOOST_DIR/lib/$BOOST_HOST_ARCH
BOOST_FLAGS="--with-libraries=python --prefix=$LOCAL_BOOST_DIR --libdir=$LOCAL_BOOST_DIR/lib/$BOOST_HOST_ARCH"
BOOST_FLAGS="$BOOST_FLAGS --with-python-root=$PYTHON_DIR"
BOOST_FLAGS="$BOOST_FLAGS --with-python=$PYTHON_BIN"
PYTHON_MAJOR_MINOR_VERSION=`$PYTHON_BIN --version 2>&1 | cut -f2 -d ' ' | cut -f1,2 -d '.'`
PYTHON_INCLUDE_DIR=$PYTHON_DIR/include/python${PYTHON_MAJOR_MINOR_VERSION}m
if [ ! -d $PYTHON_INCLUDE_DIR ]; then
    PYTHON_INCLUDE_DIR=$PYTHON_DIR/include/python${PYTHON_MAJOR_MINOR_VERSION}
    if [ ! -d $PYTHON_INCLUDE_DIR ]; then
        echo "Python include directory not found."
        exit 1
    fi
fi

echo "Executing: ./bootstrap.sh $BOOST_FLAGS"
./bootstrap.sh $BOOST_FLAGS || exit 1

# We must correct project-config.jam as bootstrap creates incomplete python entry
echo "Reconfiguring boost python build"
PYTHON_CONFIG="using python : $PYTHON_MAJOR_MINOR_VERSION : $PYTHON_BIN : $PYTHON_INCLUDE_DIR : $PYTHON_DIR/lib : ;"
cmd="cat project-config.jam | sed 's?using python.*?$PYTHON_CONFIG?' > project-config.jam2 && mv project-config.jam2 project-config.jam" 
echo $cmd
eval $cmd || exit 1

# Determine if C++11 is needed (anything higher than 1.81.0)
BOOST_VERSION_NUMBER=`cat boost/version.hpp  | grep BOOST_VERSION | grep \#define | grep -v HPP | awk '{print $NF}'`
PVAPY_USE_CPP11=${PVAPY_USE_CPP11:-0}
if [ $BOOST_VERSION_NUMBER -gt 108100 -o $PVAPY_USE_CPP11 -gt 0 ] ; then
    echo "Building boost python, using C++11"
    ./b2 cxxflags="-std=c++11" || exit 1
else
    echo "Building boost python"
    ./b2 || exit 1
fi
./b2 install || exit 1

echo "Creating library symlinks"
mkdir -p $PREFIX/lib/$BOOST_HOST_ARCH
cd $PREFIX/lib/$BOOST_HOST_ARCH
BOOST_LIBRARIES=`find $LOCAL_BOOST_DIR/lib/$BOOST_HOST_ARCH -name '*.so*' -o -name '*.dylib*'`
for f in $BOOST_LIBRARIES; do
    libFile=`basename $f`
    echo "Linking library file: $libFile"
    rm -f $libFile && ln -s $LOCAL_BOOST_DIR/lib/$BOOST_HOST_ARCH/$libFile .
done

