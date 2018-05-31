#!/bin/sh
#
# pvaPy sh setup script
#
# sets or modifies PATH, LD_LIBRARY_PATH, PYTHONPATH and PVA_PY_SPHINX_BUILD 
# environment variables
#

export PATH=PVA_PY_PYTHON_DIR:$PATH

if test -z "$LD_LIBRARY_PATH" ; then
    export LD_LIBRARY_PATH=PVA_PY_LD_LIBRARY_PATH
else
    export LD_LIBRARY_PATH=PVA_PY_LD_LIBRARY_PATH:$LD_LIBRARY_PATH
fi

if test -z "$PYTHONPATH" ; then
    export PYTHONPATH=PVA_PY_PYTHONPATH
else
    export PYTHONPATH=PVA_PY_PYTHONPATH:$PYTHONPATH
fi

export PVA_PY_SPHINX_BUILD=PVA_PY_SPHINX_BUILD

