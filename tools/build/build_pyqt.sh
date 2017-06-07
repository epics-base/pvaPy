#!/bin/sh

# Builds PyQt

MY_DIR=`dirname $0` && cd $MY_DIR && MY_DIR=`pwd`
ENV_FILE=$MY_DIR/build_env.sh
if [ ! -f $ENV_FILE ]; then
    echo "Cannot find $ENV_FILE."
    exit 1
fi
source $ENV_FILE


PYQT_DOWNLOAD_URL=http://sourceforge.net/projects/pyqt/files/PyQt4/PyQt-${PYQT_VERSION}/PyQt4_gpl_x11-${PYQT_VERSION}.tar.gz
SIP_DOWNLOAD_URL=https://sourceforge.net/projects/pyqt/files/sip/sip-${SIP_VERSION}/sip-${SIP_VERSION}.tar.gz

PYQT_BUILD_DIR=$BUILD_DIR/PyQt4_gpl_x11-$PYQT_VERSION
PYQT_TAR_FILE=`basename $PYQT_DOWNLOAD_URL`

SIP_BUILD_DIR=$BUILD_DIR/sip-$SIP_VERSION
SIP_TAR_FILE=`basename $SIP_DOWNLOAD_URL`

mkdir -p $BUILD_DIR

# Build sip
export PATH=$PYTHON_DIR/bin:$PATH
export LD_LIBRARY_PATH=$PYTHON_DIR/lib:$LD_LIBRARY_PATH

cd $BUILD_DIR
echo Building SIP $SIP_VERSION
rsync -arvlP $SRC_DIR/$SIP_TAR_FILE $BUILD_DIR
if [ ! -f $SIP_TAR_FILE ]; then
    wget $SIP_DOWNLOAD_URL || exit 1
fi
tar zxf $SIP_TAR_FILE
cd $SIP_BUILD_DIR
python configure.py || exit 1
make || exit 1
make install || exit 1


cd $BUILD_DIR
rsync -arvlP $SRC_DIR/$PYQT_TAR_FILE $BUILD_DIR
if [ ! -f $PYQT_TAR_FILE ]; then
    wget $PYQT_DOWNLOAD_URL || exit 1
fi
tar zxf $PYQT_TAR_FILE
cd $PYQT_BUILD_DIR
python configure.py -q /usr/bin/qmake-qt4 --confirm-license || exit 1
make || exit 1
make install || exit 1

# Copy qt library to python library directory
rsync -arvlP $PYQT_BUILD_DIR/designer/libpyqt4.so $PYTHON_DIR/lib || exit 1

# Install pyqtgraph
echo "Installing pyqtgraph"
pip install pyqtgraph || exit 1

