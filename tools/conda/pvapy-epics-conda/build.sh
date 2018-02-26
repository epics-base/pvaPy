#!/bin/sh

# Builds epics for conda

echo "Building epics"
EPICS_HOST_ARCH=`./startup/EpicsHostArch`
eval "cat configure/CONFIG_SITE | sed 's?#INSTALL_LOCATION=.*?INSTALL_LOCATION=$PREFIX?' > configure/CONFIG_SITE.2 && mv configure/CONFIG_SITE.2 configure/CONFIG_SITE" 
make -j
make install 

echo "Preparing epics installation"
rsync -arvlP startup $PREFIX/
cd $PREFIX/bin
for f in `ls -c1 ./$EPICS_HOST_ARCH/*`; do 
    ln -s $f .
done





