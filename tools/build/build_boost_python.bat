CALL build_env.bat     

rmdir /q/s %BOOST_DIR%
curl -L -O https://dl.bintray.com/boostorg/release/%BOOST_VERSION%/source/boost_%BOOST_DOWNLOAD_VERSION%.tar.gz
tar zxf boost_%BOOST_DOWNLOAD_VERSION%.tar.gz
move boost_%BOOST_DOWNLOAD_VERSION% %BOOST_DIR%

cd %BOOST_DIR%

ECHO Running boostrap.bat
ECHO bootstrap.bat --with-libraries=python --with-python-root=%PYTHON_ROOT%
CALL bootstrap.bat --with-libraries=python --with-python-root=%PYTHON_ROOT%

REM ECHO using python : %PYTHON_MAJOR_VERSION% : python : %WS%/%PYTHON_DIR%/externals/pythonx86/tools/include : %WS%/%PYTHON_DIR%/PCbuild/amd64 ; >> project-config.jam
ECHO using python : %PYTHON_MAJOR_VERSION% : python : %WS%/%PYTHON_DIR%/include : %WS%/%PYTHON_DIR%/lib ; >> project-config.jam

ECHO Running b2
REM .\b2 toolset=msvc variant=release link=static threading=multi stage --with-python address-model=64
echo .\b2 toolset=msvc variant=release link=shared threading=multi stage --with-python address-model=64
.\b2 toolset=msvc variant=release link=shared threading=multi stage --with-python address-model=64
cd %WS%
