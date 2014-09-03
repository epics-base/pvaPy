export EPICS_OS_CLASS=Linux 
export EPICS_HOST_ARCH=linux-x86_64 
export EPICS_BASE=/home/sveseli/Work/CTLSDAQ/support/epics/base 
export EPICS4_DIR=/home/sveseli/Work/epics4 

inputFile=$1

g++ -I$EPICS_BASE/include -I$EPICS_BASE/include/os/$EPICS_OS_CLASS -I$EPICS4_DIR/pvDataCPP/include -I$EPICS4_DIR/pvAccessCPP/include -L$EPICS_BASE/lib/$EPICS_HOST_ARCH -L$EPICS4_DIR/pvDataCPP/lib/$EPICS_HOST_ARCH -L$EPICS4_DIR/pvAccessCPP/lib/$EPICS_HOST_ARCH $inputFile -lpvData -lpvAccess -lCom
