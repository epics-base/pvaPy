#!/bin/sh

# Builds pvapy for conda

OPT_EPICS_DIR=opt/epics
CONDA_EPICS_DIR=$PREFIX/$OPT_EPICS_DIR
EPICS_HOST_ARCH=`$CONDA_EPICS_DIR/startup/EpicsHostArch`

CONDA_BOOST_DIR=$PREFIX

OPT_PVAPY_DIR=opt/pvapy
CONDA_PVAPY_DIR=$PREFIX/$OPT_PVAPY_DIR
CONDA_PVAPY_DOC_DIR=$CONDA_PVAPY_DIR/doc
CONDA_PVAPY_EXAMPLES_DIR=$CONDA_PVAPY_DIR/examples

PYTHON_MAJOR_MINOR_VERSION=`python --version 2>&1 | cut -f2 -d ' ' | cut -f1,2 -d '.'`
PYTHON_MAJOR_VERSION=`echo $PYTHON_MAJOR_MINOR_VERSION | cut -f1 -d '.'`
PYTHON_FLAGS=""
if [ "$PYTHON_MAJOR_VERSION" = "3" ]; then
    PYTHON_FLAGS="PYTHON_VERSION=3"
fi

PVACCESS_LIB_DIR=lib/python/$PYTHON_MAJOR_MINOR_VERSION/$EPICS_HOST_ARCH

echo "Building pvapy"
make configure EPICS_BASE=$CONDA_EPICS_DIR BOOST_ROOT=$CONDA_BOOST_DIR $PYTHON_FLAGS
echo "Using BUILD_FLAGS: $BUILD_FLAGS"
make $BUILD_FLAGS

echo "Building pvapy docs"
make doc
mkdir -p $CONDA_PVAPY_DOC_DIR
rsync -arvlP documentation/sphinx/_build/html $CONDA_PVAPY_DOC_DIR/

echo "Generating pvapy modules"
SITE_PACKAGES_DIR=$PREFIX/lib/python$PYTHON_MAJOR_MINOR_VERSION/site-packages
PVACCESS_DIR=$SITE_PACKAGES_DIR/pvaccess
PVAPY_DIR=$SITE_PACKAGES_DIR/pvapy
mkdir -p $PVACCESS_DIR
mkdir -p $PVAPY_DIR

echo "from .pvaccess import *" > $PVACCESS_DIR/__init__.py
echo "from pvaccess import *" > $PVAPY_DIR/__init__.py

echo "Installing pvapy library"
#rsync -arv $PVACCESS_LIB_DIR/pvaccess.so $PREFIX/lib/python$PYTHON_MAJOR_MINOR_VERSION/lib-dynload
rsync -arv $PVACCESS_LIB_DIR/pvaccess.so $PVACCESS_DIR/



