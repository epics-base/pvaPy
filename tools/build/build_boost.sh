#!/bin/sh

# Builds python

MY_DIR=`dirname $0` && cd $MY_DIR && MY_DIR=`pwd`
ENV_FILE=$MY_DIR/build_env.sh
if [ ! -f $ENV_FILE ]; then
    echo "Cannot find $ENV_FILE."
    exit 1
fi
source $ENV_FILE


BOOST_DOWNLOAD_VERSION=`echo ${BOOST_VERSION} | sed 's?\.?_?g'`
BOOST_DOWNLOAD_URL=https://sourceforge.net/projects/boost/files/boost/$BOOST_VERSION/boost_${BOOST_DOWNLOAD_VERSION}.tar.gz

BOOST_INSTALL_DIR=$BOOST_DIR

export PATH=$PYTHON_DIR/bin:$PATH
export LD_LIBRARY_PATH=$PYTHON_DIR/lib:$LD_LIBRARY_PATH:$BOOST_DIR/lib/$EPICS_HOST_ARCH
PYTHON_MAJOR_MINOR_VERSION=`python --version 2>&1 | cut -f2 -d ' ' | cut -f1,2 -d '.'`

mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Build python. 
echo Building boost $BOOST_VERSION
BOOST_TAR_FILE=`basename $BOOST_DOWNLOAD_URL`
if [ ! -f $BOOST_TAR_FILE ]; then
    wget $BOOST_DOWNLOAD_URL || exit 1
fi
BOOST_BUILD_DIR=`echo $BOOST_TAR_FILE | sed 's?.tar.gz??g'`

tar xf $BOOST_TAR_FILE
cd $BOOST_BUILD_DIR

echo ./bootstrap.sh --with-libraries=python --prefix=$BOOST_INSTALL_DIR --with-python-root=$PYTHON_DIR --with-python=$PYTHON_DIR/bin/python --libdir=$BOOST_INSTALL_DIR/lib/$EPICS_HOST_ARCH 
#./bootstrap.sh --with-libraries=python --prefix=$BOOST_INSTALL_DIR --with-python-root=$PYTHON_DIR --with-python=$PYTHON_DIR/bin/python --libdir=$BOOST_INSTALL_DIR/lib/$EPICS_HOST_ARCH || exit 1
./bootstrap.sh --prefix=$BOOST_INSTALL_DIR --with-python-root=$PYTHON_DIR --with-python=$PYTHON_DIR/bin/python --libdir=$BOOST_INSTALL_DIR/lib/$EPICS_HOST_ARCH || exit 1

# We must correct project-config.jam as bootsrap creates incomplete python entry
echo "Reconfiguring python build"
PYTHON_CONFIG="using python : $PYTHON_MAJOR_MINOR_VERSION : $PYTHON_DIR/bin/python : $PYTHON_DIR/include/python$PYTHON_MAJOR_MINOR_VERSION : $PYTHON_DIR/lib : ;"
cmd="cat project-config.jam | sed 's?using python.*?$PYTHON_CONFIG?' > project-config.jam2 && mv project-config.jam2 project-config.jam" 
echo $cmd
eval $cmd || exit 1

#./b2 --with-python python-debugging=off threading=multi variant=release address-model=64 || exit 1
./b2 || exit 1
./b2 install || exit 1


# Create link
cd $OPT_DIR && rm -f boost && ln -s `basename ${BOOST_INSTALL_DIR}` boost


