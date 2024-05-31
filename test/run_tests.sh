#!/bin/sh

TEST_SERVER_RUNTIME=30
TEST_FILES_ALL=`ls -c1 test*.py | grep -v testUtility | grep -v testServer.py`
TEST_FILES_PVACCESS=`grep pvaccess test*.py | cut -f1 -d':' | sort -u | grep -v testUtility | grep -v testServer.py`

TEST_FILES=$@
RUN_SPECIFIC_TESTS=`echo $TEST_FILES | grep test`
if [ -z "$TEST_FILES" ]; then
    TEST_FILES=$TEST_FILES_PVACCESS
elif [ -z "$RUN_SPECIFIC_TESTS" ]; then
    TEST_FILES=$TEST_FILES_ALL
    TEST_SERVER_RUNTIME=60
fi

echo "Starting test server with runtime $TEST_SERVER_RUNTIME"
python testServer.py $TEST_SERVER_RUNTIME &

echo "Running tests:"
echo $TEST_FILES
echo

pytest -s $TEST_FILES
EXIT_STATUS=$?

wait
echo "Tests done, exit status: $EXIT_STATUS"
if [ $EXIT_STATUS -eq 0 ]; then
    echo "All tests passed"
fi
exit $EXIT_STATUS
