#!/bin/sh

TEST_DIR=../../../test
cd $TEST_DIR

echo "Starting test server"
python testServer.py 30  &

echo "Starting tests"
nose2 -v 

wait 
echo "Tests done"
