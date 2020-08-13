#!/bin/sh
#
# pvaPy sh setup script
#
# sets or modifies PATH, LD_LIBRARY_PATH, PYTHONPATH and PVAPY_SPHINX_BUILD 
# environment variables
#

export PATH=PVAPY_PYTHON_DIR:$PATH

if test -z "$LD_LIBRARY_PATH" ; then
    export LD_LIBRARY_PATH=PVAPY_LD_LIBRARY_PATH
else
    export LD_LIBRARY_PATH=PVAPY_LD_LIBRARY_PATH:$LD_LIBRARY_PATH
fi

if test -z "$PYTHONPATH" ; then
    export PYTHONPATH=PVAPY_PYTHONPATH
else
    export PYTHONPATH=PVAPY_PYTHONPATH:$PYTHONPATH
fi

export PVAPY_SPHINX_BUILD=PVAPY_SPHINX_BUILD

