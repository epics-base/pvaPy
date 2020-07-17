@ECHO ON
SET EPICS_BASE_VERSION=7.0.4
SET TA=win64
SET WS=C:/Jenkins/%TA%-2/workspace/PVAPY
SET EPICS_BASE_DIR=epics-base-%EPICS_BASE_VERSION%

rmdir /q/s %EPICS_BASE_DIR%
curl -O https://epics.anl.gov/download/base/base-%EPICS_BASE_VERSION%.tar.gz
tar zxf base-%EPICS_BASE_VERSION%.tar.gz
move base-%EPICS_BASE_VERSION% %EPICS_BASE_DIR%

cd %EPICS_BASE_DIR%
make -j4 -O --no-print-directory
cd %WS%
