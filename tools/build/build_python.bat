CALL build_env.bat

rmdir /q/s %PYTHON_DIR%
curl -O https://www.python.org/ftp/python/%PYTHON_VERSION%/Python-%PYTHON_VERSION%.tgz
tar zxf Python-%PYTHON_VERSION%.tgz
move Python-%PYTHON_VERSION% %PYTHON_DIR%

cd %PYTHON_DIR%
SET PYTHON_CONFIGURE_OPTS="--enable-shared"
PCbuild\build.bat -e --no-tkinter -p x64
cd %WS%


