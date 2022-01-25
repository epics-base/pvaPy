#!/bin/sh

MY_DIR=`dirname $0` && cd $MY_DIR && MY_DIR=`pwd`
TOP_DIR=`cd $MY_DIR/../.. && pwd`
VERSION=${1:-dev}

cd $TOP_DIR
echo "Top directory: $TOP_DIR"
echo "Updating version string to: $VERSION"

f=$TOP_DIR/documentation/sphinx/conf.py
echo "Updating: $f"
cmd="cat $f | sed 's?version.*=.*?version = \"${VERSION}\"?' | sed 's?release.*=.*?release = \"${VERSION}\"?' > $f.2 && mv $f.2 $f"
eval "$cmd"
 
f=$TOP_DIR/pvapy/__init__.py
echo "Updating: $f"
cmd="cat $f | sed 's?__version__.*=.*?__version__ = \"${VERSION}\"?' > $f.2 && mv $f.2 $f"
eval "$cmd"
 
f=$TOP_DIR/pvaccess/__init__.py
echo "Updating: $f"
cmd="cat $f | sed 's?__version__.*=.*?__version__ = \"${VERSION}\"?' > $f.2 && mv $f.2 $f"
eval "$cmd"
 
f=$TOP_DIR/configure/BUILD.conf
echo "Updating: $f"
cmd="cat $f | sed 's?PVAPY_VERSION.*=.*?PVAPY_VERSION=${VERSION}?' > $f.2 && mv $f.2 $f"
eval "$cmd"
 
 
 
 

