#!/bin/sh

echo "Starting test server"
python testServer.py 30 &

echo "Starting tests"
nose2 -v

wait
echo "Tests done"

