#!/bin/sh

echo "Starting test server"
python testServer.py 30 &

echo "Starting tests"
nosetests -sv

wait
echo "Tests done"

