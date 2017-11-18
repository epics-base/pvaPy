#!/bin/sh

# Builds python

MY_DIR=`dirname $0` && cd $MY_DIR && MY_DIR=`pwd`
ENV_FILE=$MY_DIR/build_env.sh
if [ ! -f $ENV_FILE ]; then
    echo "Cannot find $ENV_FILE."
    exit 1
fi
source $ENV_FILE


BOOST_PYTHON3_DOWNLOAD_VERSION=`echo ${BOOST_PYTHON3_VERSION} | sed 's?\.?_?g'`
BOOST_PYTHON3_DOWNLOAD_URL=https://sourceforge.net/projects/boost/files/boost/$BOOST_PYTHON3_VERSION/boost_${BOOST_PYTHON3_DOWNLOAD_VERSION}.tar.gz

BOOST_PYTHON3_INSTALL_DIR=$BOOST_PYTHON3_DIR

export PATH=$PYTHON3_DIR/bin:$PATH
export LD_LIBRARY_PATH=$PYTHON3_DIR/lib:$LD_LIBRARY_PATH:$BOOST_PYTHON3_DIR/lib/$EPICS_HOST_ARCH
PYTHON3_MAJOR_MINOR_VERSION=`python3 --version 2>&1 | cut -f2 -d ' ' | cut -f1,2 -d '.'`

mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Build python. 
echo Building boost $BOOST_PYTHON3_VERSION
BOOST_PYTHON3_TAR_FILE=`basename $BOOST_PYTHON3_DOWNLOAD_URL`
if [ ! -f $BOOST_PYTHON3_TAR_FILE ]; then
    wget $BOOST_PYTHON3_DOWNLOAD_URL || exit 1
fi
BOOST_PYTHON3_BUILD_DIR=`echo $BOOST_PYTHON3_TAR_FILE | sed 's?.tar.gz??g'`

rm -rf $BOOST_PYTHON3_BUILD_DIR
tar xf $BOOST_PYTHON3_TAR_FILE
cd $BOOST_PYTHON3_BUILD_DIR

echo ./bootstrap.sh --with-libraries=python --prefix=$BOOST_PYTHON3_INSTALL_DIR --with-python-root=$PYTHON3_DIR --with-python=$PYTHON3_DIR/bin/python3 --libdir=$BOOST_PYTHON3_INSTALL_DIR/lib/$EPICS_HOST_ARCH 
#./bootstrap.sh --with-libraries=python --prefix=$BOOST_PYTHON3_INSTALL_DIR --with-python-root=$PYTHON3_DIR --with-python=$PYTHON3_DIR/bin/python3 --libdir=$BOOST_PYTHON3_INSTALL_DIR/lib/$EPICS_HOST_ARCH || exit 1
./bootstrap.sh --prefix=$BOOST_PYTHON3_INSTALL_DIR --with-python-root=$PYTHON3_DIR --with-python=$PYTHON3_DIR/bin/python3 --libdir=$BOOST_PYTHON3_INSTALL_DIR/lib/$EPICS_HOST_ARCH || exit 1

# We must correct project-config.jam as bootsrap creates incomplete python entry
echo "Reconfiguring python build"
PYTHON3_CONFIG="using python : $PYTHON3_MAJOR_MINOR_VERSION : $PYTHON3_DIR/bin/python3 : $PYTHON3_DIR/include/python${PYTHON3_MAJOR_MINOR_VERSION}m : $PYTHON3_DIR/lib : ;"
cmd="cat project-config.jam | sed 's?using python.*?$PYTHON3_CONFIG?' > project-config.jam2 && mv project-config.jam2 project-config.jam" 
echo $cmd
eval $cmd || exit 1

#./b2 --with-python python-debugging=off threading=multi variant=release address-model=64 || exit 1
./b2 || exit 1
./b2 install || exit 1


# Create link
cd $OPT_DIR && rm -f boost-python3 && ln -s `basename ${BOOST_PYTHON3_INSTALL_DIR}` boost-python3


