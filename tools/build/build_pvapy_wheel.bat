CALL build_env.bat

rmdir /q/s %PVAPY_WHEEL_DIR%
xcopy pvapy-pip %PVAPY_WHEEL_DIR% /I/E

copy %PVAPY_DIR%\bin\windows-x64\pvaccess.dll %PVAPY_WHEEL_DIR%\pvaccess
move %PVAPY_WHEEL_DIR%\pvaccess\pvaccess.dll %PVAPY_WHEEL_DIR%\pvaccess\pvaccess.pyd
copy %BOOST_DIR%\stage\lib\*.dll %PVAPY_WHEEL_DIR%\pvaccess
copy %EPICS_BASE_DIR%\bin\windows-x64\*.dll %PVAPY_WHEEL_DIR%\pvaccess

cd %PVAPY_WHEEL_DIR%

python setup.py bdist_wheel

cd %WS%
