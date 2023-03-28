'''
Test HDF5 AD Image Writer.
'''
from unittest.mock import Mock
import tempfile
import os
import sys
import pylint.lint

from pvapy.hpc.hdf5AdImageWriter import Hdf5AdImageWriter

def testLint(monkeypatch):
    ''' Test for linting errors '''
    monkeypatch.setattr(sys, 'exit', Mock())
    pylint_opts = ['pvapy.hpc.hdf5AdImageWriter', '--disable=all', '--enable=E,F', '--generated-members="pva.*,adImageUtility.*"']
    pylint.lint.Run(pylint_opts)
    sys.exit.assert_called_once_with(0)
