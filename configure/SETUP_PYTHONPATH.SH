#!/bin/sh
#
# pvaPy sh setup script
#
# modifies PYTHONPATH environment variable
#

if test -z "$PYTHONPATH" ; then
    export PYTHONPATH=PVA_PY_PYTHONPATH
else
    export PYTHONPATH=PVA_PY_PYTHONPATH:$PYTHONPATH
fi
