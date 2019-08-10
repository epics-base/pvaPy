#!/bin/sh

echo "Starting test server"
cd test
python testServer.py 10  &

echo "Starting tests"
nosetests -v testPvObject.py
nosetests -v testChannelPut.py
nosetests -v testMultiChannel.py

wait 
echo "Tests done"
