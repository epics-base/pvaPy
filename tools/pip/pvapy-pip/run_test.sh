#!/bin/sh

TEST_DIR=../../../test
cd $TEST_DIR

echo "Starting test server"
python testServer.py 60  &

echo "Starting tests"
TEST_FILES=`ls -c1 test*.py | grep -v testUtility | grep -v testServer.py`
pytest -sx $TEST_FILES

wait 
echo "Tests done"
