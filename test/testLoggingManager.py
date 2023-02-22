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
    monkeypatch.setattr(sys, "exit", Mock())
    pylint_opts = ["pvapy.utility.loggingManager", "--disable=all", "--enable=E,F", "--generated-members=loggingManager.*"]
    pylint.lint.Run(pylint_opts)
    sys.exit.assert_called_once_with(0)

def testStdoutLogging():
    ''' Test stdout logging '''
    f = tempfile.NamedTemporaryFile(mode='w+t', delete=False)
    stdout = sys.stdout
    sys.stdout = f
    logger = LoggingManager.getLogger('stdouttest')
    LoggingManager.setLogLevel('debug')
    originalMessage = f'p{RandomUtility.getRandomString(20)}'
    logger.debug(originalMessage)
    f.close()
    sys.stdout = stdout
    logMessage = open(f.name).read()
    print(f'Original message: {originalMessage}')
    print(f'Log message: {logMessage}')
    os.remove(f.name)
    assert originalMessage in logMessage

def testFileLogging():
    ''' Test file logging '''
    f = tempfile.NamedTemporaryFile(mode='w+t', delete=False)
    logger = LoggingManager.getLogger('filetest', logLevel='debug', logFile=f.name)
    originalMessage = f'p{RandomUtility.getRandomString(20)}'
    logger.debug(originalMessage)
    f.close()
    logMessage = open(f.name, encoding='ascii').read()
    print(f'Original message: {originalMessage}')
    print(f'Log message: {logMessage}')
    os.remove(f.name)
    assert originalMessage in logMessage

