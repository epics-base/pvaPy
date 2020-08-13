#!/bin/sh
#
# pvaPy sh setup script
#
# modifies PYTHONPATH environment variable
#

if test -z "$PYTHONPATH" ; then
    export PYTHONPATH=PVAPY_PYTHONPATH
else
    export PYTHONPATH=PVAPY_PYTHONPATH:$PYTHONPATH
fi
