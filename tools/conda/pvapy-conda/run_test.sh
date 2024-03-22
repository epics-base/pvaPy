#!/bin/sh

TEST_DIR=test
cd $TEST_DIR

echo "Starting test server"
python testServer.py 30 &

echo "Starting tests"
TEST_FILES=`ls -c1 test*.py | grep -v testUtility | grep -v testServer.py`
EXIT_STATUS=0
FAILED_TEST=""
for T in $TEST_FILES; do
    echo "Running test: $T"
    pytest -sx $T
    if [ $? -ne 0 ]; then
        EXIT_STATUS=1
        FAILED_TESTS="$FAILED_TESTS $T"
    fi
done

wait
echo "Tests done, exit status: $EXIT_STATUS"
if [ $EXIT_STATUS -eq 0 ]; then
    echo "All tests passed"
else
    echo "Failed tests: $FAILED_TESTS"
fi
exit $EXIT_STATUS

