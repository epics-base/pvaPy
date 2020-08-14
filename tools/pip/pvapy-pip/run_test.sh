#!/bin/sh

TEST_DIR=../../../test
cd $TEST_DIR

echo "Starting test server"
python testServer.py 10  &

echo "Starting tests"
nosetests -v testPvObject.py
nosetests -v testNtTypes.py
nosetests -v testChannelPut.py
nosetests -v testMultiChannel.py

wait 
echo "Tests done"
