'''
Test AD Image Utility.
'''
from unittest.mock import Mock
import tempfile
import sys
import pylint.lint

from pvapy.utility.adImageUtility import AdImageUtility

def testLint(monkeypatch):
    ''' Test for linting errors '''
    monkeypatch.setattr(sys, 'exit', Mock())
    # Disable E1101 due to a false result for numpy.ndarray.__array__interface__ method
    pylint_opts = ['pvapy.utility.adImageUtility', '--disable=all', '--enable=E,F', '--disable=E1101', '--generated-members="pva.*"']
    pylint.lint.Run(pylint_opts)
    sys.exit.assert_called_once_with(0)
