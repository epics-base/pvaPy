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

echo "Building pvapy $PVAPY_VERSION"
echo "Recreating build directory $BUILD_DIR"
#rm -rf $BUILD_DIR
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Download.
PVAPY_TAR_FILE=pvaPy-$PVAPY_VERSION.tar.gz
PVAPY_DOWNLOAD_URL=https://github.com/epics-base/pvaPy/archive/$PVAPY_VERSION.tar.gz
if [ ! -f $PVAPY_TAR_FILE ]; then
    echo "Downloading $PVAPY_TAR_FILE"
    curl -Ls -o $PVAPY_TAR_FILE -w %{url_effective} $PVAPY_DOWNLOAD_URL
    if [ $? -ne 0 ]; then
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
if [ ! -d $PVAPY_BUILD_DIR ]; then
    tar zxf $PVAPY_TAR_FILE || exit 1
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
echo "Using BUILD_FLAGS: $BUILD_FLAGS"
cd $PVAPY_BUILD_DIR
echo "Running: make configure $PVAPY_FLAGS"
make configure $PVAPY_FLAGS || exit 1
echo "Runinng: make $BUILD_FLAGS"
make $BUILD_FLAGS || exit 1

echo "Building pvapy docs"
make doc || exit 1
mkdir -p $PVACCESS_DOC_DIR
rsync -arvl documentation/sphinx/_build/html $PVACCESS_DOC_DIR/

echo "Copying data files"
rsync -arvl README.md $PVACCESS_DOC_DIR/

echo "Installing pvapy library"
rsync -arv $PVACCESS_BUILD_LIB_DIR/$PVACCESS_LIB $PVACCESS_DIR/

echo "Copying module files"
rsync -arvl pvapy pvaccess $TOP

echo "Updating python module init files"
INIT_FILE=$PVACCESS_DIR/__init__.py
cmd="cat $INIT_FILE | sed 's?__version__.*=.*?__version__ = \"$PVAPY_VERSION\"?' > $INIT_FILE.2 && mv $INIT_FILE.2 $INIT_FILE"
eval $cmd

echo "Copying dependencies"
EPICS_LIBS=`ls -c1 $EPICS_BASE_DIR/lib/$EPICS_HOST_ARCH/*.so`
BOOST_LIBS=`ls -c1 $BOOST_DIR/lib/$BOOST_HOST_ARCH/*.so`
mkdir -p $PVACCESS_LIB_DIR/$EPICS_HOST_ARCH
for lib in $EPICS_LIBS $BOOST_LIBS; do
    rsync -arvl ${lib}* $PVACCESS_LIB_DIR/$EPICS_HOST_ARCH/
done

# Fix rpath
cd $PVACCESS_DIR
newRpath="\$ORIGIN/lib/$EPICS_HOST_ARCH"
echo "Setting RPATH for file $PVACCESS_LIB to $newRpath"
chmod u+w $PVACCESS_LIB
patchelf --set-rpath $newRpath $PVACCESS_LIB  || exit 1

# Remove libpython from needed libraries
echo "Removing libpython as dependency from $PVACCESS_LIB"
patchelf --remove-needed `basename $PYTHON_LIB` $PVACCESS_LIB

echo "Stripping all libraries"
cd $PVACCESS_DIR
strip -v $PVACCESS_LIB
cd $PVACCESS_LIB_DIR/$EPICS_HOST_ARCH/
strip -v `find . -type f` 

# Done
cd $CURRENT_DIR



