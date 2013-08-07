#!/bin/bash
# FIXME AK to refactor from write once mode
if [ $# -ne 3 ]
then
    echo "Usage: ./runTest N test disturber"
    exit -1
fi

if [ -x $3 ]
then
echo $3
 $3 >/dev/null 2>&1 &
fi

test -e bin/test.x && NAME=`mktemp` && for i in `seq 1 $1`; do taskset -c 1 bin/test.x >> $NAME ; done && echo -n "Test:" $2 ":" && echo `../../tools/bin/results.sh $NAME` 

killall -9 $3 >/dev/null 2>&1

