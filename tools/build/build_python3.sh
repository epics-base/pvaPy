#!/bin/sh

# Builds python

MY_DIR=`dirname $0` && cd $MY_DIR && MY_DIR=`pwd`
ENV_FILE=$MY_DIR/build_env.sh
if [ ! -f $ENV_FILE ]; then
    echo "Cannot find $ENV_FILE."
    exit 1
fi
source $ENV_FILE


PYTHON3_DOWNLOAD_URL=https://www.python.org/ftp/python/$PYTHON3_VERSION/Python-${PYTHON3_VERSION}.tar.xz

PYTHON3_BUILD_DIR=$BUILD_DIR/Python-$PYTHON3_VERSION
PYTHON3_INSTALL_DIR=$PYTHON3_DIR

mkdir -p $BUILD_DIR

# Hack for libBLT, which does not seem to be recognized by the setup.py
# This assumes 64-bit architecture.
mkdir -p $PYTHON3_INSTALL_DIR/lib
cd $PYTHON3_INSTALL_DIR/lib
rm -f libBLT.so && ln -s /usr/lib64/libBLT24.so libBLT.so

# Build python. 
#export PATH=$PYTHON3_INSTALL_DIR/bin/$EPICS_HOST_ARCH:$PATH
export PATH=$PYTHON3_INSTALL_DIR/bin:$PATH
export PATH=$PYTHON3_INSTALL_DIR/lib:$PATH
#export LD_LIBRARY_PATH=$TK_DIR/lib:$TCL_DIR/lib:$PYTHON3_INSTALL_DIR/lib/$EPICS_HOST_ARCH:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$TK_DIR/lib:$TCL_DIR/lib:$PYTHON3_INSTALL_DIR/lib:$LD_LIBRARY_PATH
cd $BUILD_DIR
echo Building python $PYTHON3_VERSION
PYTHON3_TAR_FILE=`basename $PYTHON3_DOWNLOAD_URL`
if [ ! -f $PYTHON3_TAR_FILE ]; then
    wget $PYTHON3_DOWNLOAD_URL || exit 1
fi
tar xf $PYTHON3_TAR_FILE
cd $PYTHON3_BUILD_DIR
./configure --prefix=$PYTHON3_INSTALL_DIR --enable-shared --with-system-ffi 
#    --bindir=$PYTHON3_INSTALL_DIR/bin/$EPICS_HOST_ARCH \
#    --libdir=$PYTHON3_INSTALL_DIR/lib/$EPICS_HOST_ARCH 
make && make install || exit 1

# Install sphinx
echo "Installing Python Packages"
#cd $BUILD_DIR
#cp $SRC_DIR/ez_setup.py .
#python ez_setup.py || exit 1
if [ ! -f $PYTHON3_DIR/bin/pip3 ]; then
    echo "Installing pip"
    wget https://bootstrap.pypa.io/get-pip.py -O - | python3
fi
#easy_install pip || exit 1
pip3 install bqplot || exit 1
pip3 install cdb_api|| exit 1
pip3 install cherrypy || exit 1
pip3 install decorator || exit 1
pip3 install jupyter || exit 1
pip3 install matplotlib || exit 1
pip3 install numpy || exit 1
pip3 install pymongo || exit 1
pip3 install pysftp || exit 1
#pip3 install python_ldap || exit 1
pip3 install pytest || exit 1
pip3 install pytz || exit 1
pip3 install pyzmq || exit 1
pip3 install routes || exit 1
pip3 install scipy || exit 1
pip3 install sphinx || exit 1
pip3 install sqlalchemy || exit 1
#pip3 install suds || exit 1
pip3 install tzlocal || exit 1
pip3 install watchdog || exit 1
pip3 install PyQt5 || exit 1

jupyter nbextension enable --py --sys-prefix widgetsnbextension || exit 1
jupyter nbextension enable --py --sys-prefix bqplot || exit 1

# Create python link
cd $OPT_DIR && rm -f python3 && ln -s python3-${PYTHON3_VERSION} python3


