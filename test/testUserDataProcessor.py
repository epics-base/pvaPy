'''
Test User Data Processor
'''
from unittest.mock import Mock
import sys
import pylint.lint

from pvapy.hpc.userDataProcessor import UserDataProcessor 

def testLint(monkeypatch):
    ''' Test for linting errors '''
    monkeypatch.setattr(sys, 'exit', Mock())
    pylint_opts = ['pvapy.hpc.userDataProcessor', '--disable=all', '--enable=E,F', '--generated-members="pva.*"']
    pylint.lint.Run(pylint_opts)
    sys.exit.assert_called_once_with(0)
