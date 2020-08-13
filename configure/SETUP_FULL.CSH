#!/bin/csh
#
# pvaPy csh setup script
#
# sets or modifies PATH, LD_LIBRARY_PATH, PYTHONPATH and PVAPY_SPHINX_BUILD 
# environment variables
#

setenv PATH PVAPY_PYTHON_DIR:${PATH}

if ( ! $?LD_LIBRARY_PATH ) then
    setenv LD_LIBRARY_PATH PVAPY_LD_LIBRARY_PATH
else
    setenv LD_LIBRARY_PATH PVAPY_LD_LIBRARY_PATH:${LD_LIBRARY_PATH}
endif

if ( ! $?PYTHONPATH ) then
    setenv PYTHONPATH PVAPY_PYTHONPATH
else
    setenv PYTHONPATH PVAPY_PYTHONPATH:${PYTHONPATH}
endif

setenv PVAPY_SPHINX_BUILD PVAPY_SPHINX_BUILD


