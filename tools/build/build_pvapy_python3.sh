#!/bin/bash

MY_DIR=`dirname $0` && cd $MY_DIR && MY_DIR=`pwd`
ENV_FILE=$MY_DIR/build_env.sh
if [ ! -f $ENV_FILE ]; then
    echo "Cannot find $ENV_FILE."
exit 1
fi
source $ENV_FILE

echo Building pvaPy $PVAPY_PYTHON3_VERSION for python3 
export PATH=$PYTHON3_DIR/bin:$PATH
export LD_LIBRARY_PATH=$PYTHON3_DIR/lib:$LD_LIBRARY_PATH:$BOOST_PYTHON3_DIR/lib/$EPICS_HOST_ARCH
PYTHON3_MAJOR_MINOR_VERSION=`python3 --version 2>&1 | cut -f2 -d ' ' | cut -f1,2 -d '.'`

PVAPY_PYTHON3_INSTALL_DIR=$PVAPY_PYTHON3_DIR
PVAPY_PYTHON3_TAR_FILE=$SRC_DIR/pvaPy-$PVAPY_PYTHON3_VERSION.tar.gz
cd $EPICS_MODULES_DIR

tar zxf $PVAPY_PYTHON3_TAR_FILE || exit 1
rm -rf pvaPy-python3-$PVAPY_PYTHON3_VERSION
mv pvaPy-$PVAPY_PYTHON3_VERSION pvaPy-python3-$PVAPY_PYTHON3_VERSION
cd pvaPy-python3-$PVAPY_PYTHON3_VERSION || exit 1

EPICS_BASE=$EPICS_BASE EPICS4_DIR=$EPICS4_DIR BOOST_ROOT=$BOOST_PYTHON3_DIR PYTHON_VERSION=3 make configure || exit 1
make || exit 1

# Create link
echo "Creating pvaPy-python3 link"
cd $EPICS_MODULES_DIR && rm -f pvaPy-python3 && ln -s pvaPy-python3-$PVAPY_PYTHON3_VERSION pvaPy-python3
