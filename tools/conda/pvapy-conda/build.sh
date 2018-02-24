#!/bin/sh

# Builds pvapy for conda

EPICS_HOST_ARCH=`$PREFIX/startup/EpicsHostArch`
PYTHON_MAJOR_MINOR_VERSION=`python --version 2>&1 | cut -f2 -d ' ' | cut -f1,2 -d '.'`
PYTHON_MAJOR_VERSION=`echo $PYTHON_MAJOR_MINOR_VERSION | cut -f1 -d '.'`
PYTHON_FLAGS=""
if [ "$PYTHON_MAJOR_VERSION" = "3" ]; then
    PYTHON_FLAGS="PYTHON_VERSION=3"
fi

make configure EPICS_BASE=$PREFIX BOOST_ROOT=$PREFIX $PYTHON_FLAGS
make -j
rsync -arvlP lib/python/$PYTHON_MAJOR_MINOR_VERSION/$EPICS_HOST_ARCH/pvaccess.so $PREFIX/lib/python$PYTHON_MAJOR_MINOR_VERSION/lib-dynload/

