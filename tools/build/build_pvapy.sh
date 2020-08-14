#!/bin/bash

MY_DIR=`dirname $0` && cd $MY_DIR && MY_DIR=`pwd`
ENV_FILE=$MY_DIR/build_env.sh
if [ ! -f $ENV_FILE ]; then
    echo "Cannot find $ENV_FILE."
exit 1
fi
source $ENV_FILE

echo Building pvaPy $PVAPY_VERSION
export PATH=$PYTHON_DIR/bin:$PATH
export LD_LIBRARY_PATH=$PYTHON_DIR/lib:$LD_LIBRARY_PATH:$BOOST_DIR/lib/$EPICS_HOST_ARCH:$BOOST_NUMPY_DIR/lib/$EPICS_HOST_ARCH
PYTHON_MAJOR_MINOR_VERSION=`python --version 2>&1 | cut -f2 -d ' ' | cut -f1,2 -d '.'`

PVAPY_INSTALL_DIR=$PVAPY_DIR
PVAPY_TAR_FILE=$SRC_DIR/pvaPy-$PVAPY_VERSION.tar.gz
cd $EPICS_MODULES_DIR

tar zxf $PVAPY_TAR_FILE || exit 1
cd pvaPy-$PVAPY_VERSION || exit 1

EPICS_BASE=$EPICS_BASE EPICS4_DIR=$EPICS4_DIR BOOST_NUMPY_DIR=$BOOST_NUMPY_DIR BOOST_ROOT=$BOOST_DIR  make configure || exit 1
make || exit 1

# Create link
echo "Creating pvaPy link"
cd $EPICS_MODULES_DIR && rm -f pvaPy && ln -s pvaPy-$PVAPY_VERSION pvaPy
