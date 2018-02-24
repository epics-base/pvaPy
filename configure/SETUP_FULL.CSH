#!/bin/csh
#
# pvaPy csh setup script
#
# modifies PATH, LD_LIBRARY_PATH and PYTHONPATH environment variables
#

setenv PATH PVA_PY_PYTHON_DIR:${PATH}

if ( ! $?LD_LIBRARY_PATH ) then
    setenv LD_LIBRARY_PATH PVA_PY_LD_LIBRARY_PATH
else
    setenv LD_LIBRARY_PATH PVA_PY_LD_LIBRARY_PATH:${LD_LIBRARY_PATH}
endif

if ( ! $?PYTHONPATH ) then
    setenv PYTHONPATH PVA_PY_PYTHONPATH
else
    setenv PYTHONPATH PVA_PY_PYTHONPATH:${PYTHONPATH}
endif

