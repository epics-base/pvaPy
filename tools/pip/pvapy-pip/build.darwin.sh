#!/bin/sh

# Builds pvapy for pip


CURRENT_DIR=`pwd`
TOP_DIR=`dirname $0` && cd $TOP_DIR && TOP_DIR=`pwd`
BUILD_CONF=$TOP_DIR/../../../configure/BUILD.conf

if [ ! -f $BUILD_CONF ]; then
    echo "$BUILD_CONF not found"
    exit 1
fi
. $BUILD_CONF

#DEFAULT_PYTHON_VERSION="2"
#if [ -z "$PYTHON_VERSION" ]; then
#    PYTHON_VERSION=$DEFAULT_PYTHON_VERSION
#fi

PYTHON_BIN=`which python$PYTHON_VERSION 2> /dev/null`
PYTHON_MAJOR_MINOR_VERSION=`$PYTHON_BIN --version 2>&1 | cut -f2 -d ' ' | cut -f1,2 -d '.'`
PYTHON_MAJOR_VERSION=`echo $PYTHON_MAJOR_MINOR_VERSION | cut -f1 -d '.'`

BUILD_DIR=$TOP_DIR/build
BUILD_SAVE_DIR=$TOP_DIR/../build
PVAPY_DIR=$TOP_DIR/pvapy
PVACCESS_DIR=$TOP_DIR/pvaccess
PVACCESS_DOC_DIR=$PVACCESS_DIR/doc
PVACCESS_LIB_DIR=$PVACCESS_DIR/lib
PVAPY_BUILD_DIR=$BUILD_DIR/pvaPy-$PVAPY_VERSION
EPICS_BASE_DIR=$BUILD_SAVE_DIR/epics-base-${EPICS_BASE_VERSION}
EPICS_HOST_ARCH=`$EPICS_BASE_DIR/startup/EpicsHostArch`
BOOST_DIR=$BUILD_SAVE_DIR/pvapy-boost-${BOOST_VERSION}-py${PYTHON_MAJOR_MINOR_VERSION}

BOOST_HOST_ARCH=`uname | tr [A-Z] [a-z]`-`uname -m`
PVACCESS_LIB=pvaccess.so

mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Download.
echo "Building pvapy $PVAPY_VERSION"
PVAPY_TAR_FILE=pvaPy-$PVAPY_VERSION.tar.gz
PVAPY_DOWNLOAD_URL=https://github.com/epics-base/pvaPy/archive/$PVAPY_VERSION.tar.gz
if [ ! -f $PVAPY_TAR_FILE ]; then
    echo "Downloading $PVAPY_TAR_FILE"
    #wget -q $PVAPY_DOWNLOAD_URL && mv $PVAPY_VERSION.tar.gz $PVAPY_TAR_FILE 
    curl -Ls -o $PVAPY_TAR_FILE -w %{url_effective} $PVAPY_DOWNLOAD_URL 
    isTar=`file $PVAPY_TAR_FILE | grep gzip`
    # Curl will not exit with non-zero exit if there is no requested file
    if [ -z "$isTar" ]; then
        rm $PVAPY_TAR_FILE
        PVAPY_GIT_URL=https://github.com/epics-base/pvaPy
        echo "$PVAPY_TAR_FILE does not exist, using git repository $PVAPY_GIT_URLi, branch $PVAPY_GIT_VERSION"
        if [ ! -d $PVAPY_BUILD_DIR ]; then
            git clone $PVAPY_GIT_URL $PVAPY_BUILD_DIR
        fi
        cd $PVAPY_BUILD_DIR
        git checkout $PVAPY_GIT_VERSION
    else
        tar zxf $PVAPY_TAR_FILE || exit 1
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
export LD_LIBRARY_PATH=$PYTHON_DIR/lib:$LD_LIBRARY_PATH:$BOOST_DIR/lib/$EPICS_HOST_ARCH:$PVAPY_DIR/lib/$EPICS_HOST_ARCH

PYTHON_LIB=`ls -c1 $PYTHON_DIR/lib/libpython${PYTHON_MAJOR_MINOR_VERSION}*.so.* 2> /dev/null` 

PVAPY_FLAGS=""
if [ "$PYTHON_MAJOR_VERSION" = "3" ]; then
    PVAPY_FLAGS="PYTHON_VERSION=3"
fi
PVAPY_FLAGS="EPICS_BASE=$EPICS_BASE_DIR BOOST_ROOT=$BOOST_DIR PVAPY_ROOT=$PVAPY_DIR $PVAPY_FLAGS"
PVACCESS_BUILD_LIB_DIR=$PVAPY_BUILD_DIR/lib/python/$PYTHON_MAJOR_MINOR_VERSION/$EPICS_HOST_ARCH

echo "Building pvapy"
cd $PVAPY_BUILD_DIR
make configure $PVAPY_FLAGS || exit 1
make -j || exit 1

echo "Installing pvapy library"
mkdir -p $PVACCESS_DOC_DIR
mkdir -p $PVAPY_DIR
rsync -ar $PVACCESS_BUILD_LIB_DIR/$PVACCESS_LIB $PVACCESS_DIR/

echo "Copying data files"
rsync -arvl README.md $PVACCESS_DOC_DIR/

echo "Generating python module init files"
echo "from .pvaccess import *" > $PVACCESS_DIR/__init__.py
echo "from pvaccess import *" > $PVAPY_DIR/__init__.py

echo "Copying dependencies"
EPICS_LIBS=`ls -c1 $EPICS_BASE_DIR/lib/$EPICS_HOST_ARCH/*.dylib`
mkdir -p $PVACCESS_LIB_DIR/$EPICS_HOST_ARCH
mkdir -p $PVAPY_BUILD_DIR/lib/$EPICS_HOST_ARCH
for lib in $EPICS_LIBS; do
    rsync -arl ${lib}* $PVACCESS_LIB_DIR/$EPICS_HOST_ARCH/
    rsync -arl ${lib}* $PVAPY_BUILD_DIR/lib/$EPICS_HOST_ARCH/
done
BOOST_LIBS=`ls -c1 $BOOST_DIR/lib/$BOOST_HOST_ARCH/*.dylib`
mkdir -p $PVACCESS_LIB_DIR/$BOOST_HOST_ARCH
mkdir -p $PVAPY_BUILD_DIR/lib/$BOOST_HOST_ARCH
for lib in $BOOST_LIBS; do
    rsync -arl ${lib}* $PVACCESS_LIB_DIR/$BOOST_HOST_ARCH/
    rsync -arl ${lib}* $PVAPY_BUILD_DIR/lib/$BOOST_HOST_ARCH/
done

# Fix libraries: first one to build documentation, second one for packaging
echo "Fixing built pvaccess.so for doc build"
cd $PVACCESS_BUILD_LIB_DIR
chmod u+w $PVACCESS_LIB
for f in $EPICS_LIBS; do
    f2=`basename $f`
    oldId="@loader_path/../../lib/$EPICS_HOST_ARCH/$f2"
    newId="@loader_path/../../../../lib/$EPICS_HOST_ARCH/$f2"
    install_name_tool -change "$oldId" "$newId" $PVACCESS_LIB 
done
for f in $BOOST_LIBS; do
    f2=`basename $f`
    oldId="@loader_path/../../lib/$BOOST_HOST_ARCH/$f2"
    newId="@loader_path/../../../../lib/$BOOST_HOST_ARCH/$f2"
    install_name_tool -change "$oldId" "$newId" $PVACCESS_LIB 
done

echo "Fixing pvaccess.so for packaging"
cd $PVACCESS_DIR
chmod u+w $PVACCESS_LIB
for f in $EPICS_LIBS; do
    f2=`basename $f`
    oldId="@loader_path/../../lib/$EPICS_HOST_ARCH/$f2"
    newId="@loader_path/./lib/$EPICS_HOST_ARCH/$f2"
    install_name_tool -change "$oldId" "$newId" $PVACCESS_LIB 
done
for f in $BOOST_LIBS; do
    f2=`basename $f`
    oldId="@loader_path/../../lib/$BOOST_HOST_ARCH/$f2"
    newId="@loader_path/./lib/$BOOST_HOST_ARCH/$f2"
    install_name_tool -change "$oldId" "$newId" $PVACCESS_LIB 
done

cd $PVAPY_BUILD_DIR
echo "Building pvapy docs"
make doc || exit 1
rsync -arvl documentation/sphinx/_build/html $PVACCESS_DOC_DIR/

# Done
cd $CURRENT_DIR



