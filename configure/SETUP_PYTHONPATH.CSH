#!/bin/csh
#
# pvaPy csh setup script
#
# modifies PYTHONPATH environment variable
#

if ( ! $?PYTHONPATH ) then
    setenv PYTHONPATH PVAPY_PYTHONPATH
else
    setenv PYTHONPATH PVAPY_PYTHONPATH:${PYTHONPATH}
endif

