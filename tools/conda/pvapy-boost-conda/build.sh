#!/bin/sh

# Builds pvapy-boost for conda

CONDA_BOOST_DIR=$PREFIX

PYTHON_MAJOR_MINOR_VERSION=`python --version 2>&1 | cut -f2 -d ' ' | cut -f1,2 -d '.'`
PYTHON_MAJOR_VERSION=`echo $PYTHON_MAJOR_MINOR_VERSION | cut -f1 -d '.'`
PYTHON_INCLUDE_DIR=`ls -d $PREFIX/include/python$PYTHON_MAJOR_MINOR_VERSION*`
PYTHON=python
if [ "$PYTHON_MAJOR_VERSION" = "3" ]; then
    PYTHON=python3
fi
PVAPY_USE_CPP11=${PVAPY_USE_CPP11:-0}
echo "Using C++11: $PVAPY_USE_CPP11"

./bootstrap.sh --with-libraries=python --prefix=$CONDA_BOOST_DIR --with-python-root=$PREFIX --with-python=$PREFIX/bin/$PYTHON || exit 1

# We must correct project-config.jam as bootstrap creates incomplete python entry
echo "Reconfiguring boost python build"
PYTHON_CONFIG="using python : $PYTHON_MAJOR_MINOR_VERSION : $PREFIX/bin/$PYTHON : $PYTHON_INCLUDE_DIR : $PREFIX/lib : ;"
cmd="cat project-config.jam | sed 's?using python.*?$PYTHON_CONFIG?' > project-config.jam2 && mv project-config.jam2 project-config.jam" 
echo $cmd
eval $cmd || exit 1

echo "Building boost python"
if [ $PVAPY_USE_CPP11 -gt 0 ]; then
    ./b2 cxxflags="-std=c++11" || exit 1
else
    ./b2 || exit 1
fi
./b2 install || exit 1

