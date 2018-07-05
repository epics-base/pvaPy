#!/bin/sh

# Builds pvapy-boost for pip

TOP=`dirname $0` && cd $TOP && TOP=`pwd`
BUILD_DIR=$TOP/build
BOOST_DIR=$TOP/pvapy-boost
BOOST_HOST_ARCH=`uname | tr [A-Z] [a-z]`-`uname -m`
DEPLOY_CONF=$TOP/../../../configure/DEPLOY.conf
if [ ! -f $DEPLOY_CONF ]; then
    echo "$DEPLOY_CONF not found"
    exit 1
fi
. $DEPLOY_CONF

DEFAULT_PYTHON_VERSION="2"
if [ -z "$PYTHON_VERSION" ]; then
    PYTHON_VERSION=$DEFAULT_PYTHON_VERSION
fi

BOOST_LIB_LINK_SUFFIX=""
if [ "$PYTHON_VERSION" = "3" ]; then
    BOOST_LIB_LINK_SUFFIX="3"
fi

BOOST_DOWNLOAD_VERSION=`echo ${BOOST_VERSION} | sed 's?\.?_?g'`
BOOST_TAR_FILE=boost_${BOOST_DOWNLOAD_VERSION}.tar.gz
BOOST_DOWNLOAD_URL=https://sourceforge.net/projects/boost/files/boost/$BOOST_VERSION/${BOOST_TAR_FILE}/download

mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Download.

echo "Building boost $BOOST_VERSION"
if [ ! -f $BOOST_TAR_FILE ]; then
    #wget $BOOST_DOWNLOAD_URL || exit 1
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
else
    if [ -z "$PYTHON_BIN" ]; then
        echo "Python executable not found."
        exit 1
    fi
    PYTHON_DIR=`dirname \`dirname $PYTHON_BIN\``
fi
export PATH=$PYTHON_DIR/bin:$PATH
export LD_LIBRARY_PATH=$PYTHON_DIR/lib:$LD_LIBRARY_PATH:$BOOST_DIR/lib/$BOOST_HOST_ARCH

BOOST_FLAGS="--with-libraries=python --prefix=$BOOST_DIR --libdir=$BOOST_DIR/lib/$BOOST_HOST_ARCH"
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

# We must correct project-config.jam as bootsrap creates incomplete python entry
echo "Reconfiguring python build"
PYTHON_CONFIG="using python : $PYTHON_MAJOR_MINOR_VERSION : $PYTHON_BIN : $PYTHON_INCLUDE_DIR : $PYTHON_DIR/lib : ;"
cmd="cat project-config.jam | sed 's?using python.*?$PYTHON_CONFIG?' > project-config.jam2 && mv project-config.jam2 project-config.jam" 
echo $cmd
eval $cmd || exit 1

#./b2 --with-python python-debugging=off threading=multi variant=release address-model=64 || exit 1
./b2 || exit 1
./b2 install || exit 1

# Fix libraries 
cd $BOOST_DIR/lib/$BOOST_HOST_ARCH
for lib in `find . -type f -name '*.dylib'`; do
    f=`basename $lib`
    echo "Checking file: $f (pwd: $PWD)"
    chmod u+w $f
    oldId=`otool -D $f | tail -1`
    newId="@loader_path/../../lib/$BOOST_HOST_ARCH/$f"
    echo "Modifying id $oldId => $newId"
    install_name_tool -id "$newId" $f
    for f2 in `find . -type f -name '*.dylib' | grep -v $f`; do
        chmod u+w $f2
        install_name_tool -change "$oldId" "$newId" $f2
    done
done

# Create library links
if [ ! -f libboost_python$BOOST_LIB_LINK_SUFFIX.dylib ]; then
    echo "Creating library links"
    ln -s libboost_python*.dylib libboost_python$BOOST_LIB_LINK_SUFFIX.dylib
    ln -s libboost_numpy*.dylib libboost_numpy$BOOST_LIB_LINK_SUFFIX.dylib
fi

