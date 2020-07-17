CALL build_env.bat

rmdir /q/s %PVAPY_DIR%
curl -L -o pvapy-%PVAPY_VERSION%.tar.gz https://github.com/epics-base/pvaPy/archive/%PVAPY_VERSION%.tar.gz
tar zxf pvapy-%PVAPY_VERSION%.tar.gz
move pvaPy-%PVAPY_VERSION% %PVAPY_DIR%
copy CONFIG_SITE.local.template %PVAPY_DIR%\configure\CONFIG_SITE.local
copy RELEASE.local.template %PVAPY_DIR%\configure\RELEASE.local

cd %PVAPY_DIR%/configure

perl -i -p -e "s/PYTHON_VERSION/%PYTHON_VERSION%/g" CONFIG_SITE.local
perl -i -p -e "s/PYTHON_MAJOR_VERSION/%PYTHON_MAJOR_VERSION%/g" CONFIG_SITE.local
perl -i -p -e "s/PYTHON_LIB_VERSION/%PYTHON_LIB_VERSION%/g" CONFIG_SITE.local

cd ..

make -j4

cd %WS%
