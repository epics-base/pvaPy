#!/bin/sh

echo "Starting test server"
cd test
python testServer.py 15  &

for f in PvObject ChannelPut MultiChannel; do
    echo "Starting $f tests"
    sleep 1
    nosetests -v test$f.py
done

wait 
echo "Tests done"
