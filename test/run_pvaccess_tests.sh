#!/bin/sh

echo "Starting test server"
python testServer.py 30 &

echo "Starting tests"
TEST_FILES=`grep pvaccess test*.py | cut -f1 -d':' | sort -u | grep -v testUtility | grep -v testServer.py`
pytest -sx $TEST_FILES

wait
echo "Tests done"
