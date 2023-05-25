'''
   Test logging manager.
'''
from unittest.mock import Mock
import tempfile
import os
import sys
import pylint.lint

from pvapy.utility.randomUtility import RandomUtility
from pvapy.utility.loggingManager import LoggingManager

def testLint(monkeypatch):
    ''' Test for linting errors '''
    monkeypatch.setattr(sys, 'exit', Mock())
    pylint_opts = ['pvapy.utility.loggingManager', '--disable=all', '--enable=E,F', '--generated-members=loggingManager.*']
    pylint.lint.Run(pylint_opts)
    sys.exit.assert_called_once_with(0)

def testStdoutLogging():
    ''' Test stdout logging '''
    originalMessage = f'p{RandomUtility.getRandomString(20)}'
    stdout = sys.stdout
    with tempfile.NamedTemporaryFile(mode='w+t', delete=False) as f:
        sys.stdout = f
        filePath = f.name
        logger = LoggingManager.getLogger('stdouttest')
        LoggingManager.setLogLevel('debug')
        logger.debug(originalMessage)
    sys.stdout = stdout
    print(f'Original message: {originalMessage}')
    logMessage = open(filePath).read()
    print(f'Log message: {logMessage}')
    os.remove(filePath)
    assert originalMessage in logMessage

