#!/bin/csh
#
# pvaPy csh setup script
#
# modifies PYTHONPATH environment variable
#

if ( ! $?PYTHONPATH ) then
    setenv PYTHONPATH PVA_PY_PYTHONPATH
else
    setenv PYTHONPATH PVA_PY_PYTHONPATH:${PYTHONPATH}
endif

