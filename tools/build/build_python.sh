#!/bin/sh

# Builds python

MY_DIR=`dirname $0` && cd $MY_DIR && MY_DIR=`pwd`
ENV_FILE=$MY_DIR/build_env.sh
if [ ! -f $ENV_FILE ]; then
    echo "Cannot find $ENV_FILE."
    exit 1
fi
source $ENV_FILE


PYTHON_DOWNLOAD_URL=https://www.python.org/ftp/python/$PYTHON_VERSION/Python-${PYTHON_VERSION}.tar.xz

PYTHON_BUILD_DIR=$BUILD_DIR/Python-$PYTHON_VERSION
PYTHON_INSTALL_DIR=$PYTHON_DIR

mkdir -p $BUILD_DIR

# Hack for libBLT, which does not seem to be recognized by the setup.py
# This assumes 64-bit architecture.
mkdir -p $PYTHON_INSTALL_DIR/lib
cd $PYTHON_INSTALL_DIR/lib
rm -f libBLT.so && ln -s /usr/lib64/libBLT24.so libBLT.so

# Build python. 
#export PATH=$PYTHON_INSTALL_DIR/bin/$EPICS_HOST_ARCH:$PATH
export PATH=$PYTHON_INSTALL_DIR/bin:$PATH
export PATH=$PYTHON_INSTALL_DIR/lib:$PATH
#export LD_LIBRARY_PATH=$TK_DIR/lib:$TCL_DIR/lib:$PYTHON_INSTALL_DIR/lib/$EPICS_HOST_ARCH:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$TK_DIR/lib:$TCL_DIR/lib:$PYTHON_INSTALL_DIR/lib:$LD_LIBRARY_PATH
cd $BUILD_DIR
echo Building python $PYTHON_VERSION
PYTHON_TAR_FILE=`basename $PYTHON_DOWNLOAD_URL`
if [ ! -f $PYTHON_TAR_FILE ]; then
    wget $PYTHON_DOWNLOAD_URL || exit 1
fi
tar xf $PYTHON_TAR_FILE
cd $PYTHON_BUILD_DIR
./configure --prefix=$PYTHON_INSTALL_DIR --enable-shared --with-system-ffi 
#    --bindir=$PYTHON_INSTALL_DIR/bin/$EPICS_HOST_ARCH \
#    --libdir=$PYTHON_INSTALL_DIR/lib/$EPICS_HOST_ARCH 
make && make install || exit 1

# Install sphinx
echo "Installing Python Packages"
#cd $BUILD_DIR
#cp $SRC_DIR/ez_setup.py .
#python ez_setup.py || exit 1
if [ ! -f $PYTHON_DIR/bin/pip ]; then
    echo "Installing pip"
    wget https://bootstrap.pypa.io/get-pip.py -O - | python
fi
#easy_install pip || exit 1
pip install sphinx || exit 1
pip install numpy || exit 1
pip install jupyter || exit 1
pip install matplotlib || exit 1
pip install scipy || exit 1
pip install bqplot || exit 1
jupyter nbextension enable --py --sys-prefix bqplot || exit 1

# Create python link
cd $OPT_DIR && rm -f python && ln -s python-${PYTHON_VERSION} python


