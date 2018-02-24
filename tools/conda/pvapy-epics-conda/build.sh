#!/bin/sh

# Builds epics for conda

echo "Building epics"
eval "cat configure/CONFIG_SITE | sed 's?#INSTALL_LOCATION=.*?INSTALL_LOCATION=$PREFIX?' > configure/CONFIG_SITE.2 && mv configure/CONFIG_SITE.2 configure/CONFIG_SITE" 
make -j
make install 

echo "Preparing epics installation"
rsync -arvlP startup $PREFIX/




