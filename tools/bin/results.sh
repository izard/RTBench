#!/bin/bash
# Outputs average and largest number in a log file

if [ $# -ne 1 ]
then
    echo "Usage: results FILENAME"
    exit -1
fi

if [ -f $1 ];
then
awk 'BEGIN {biggest=0 } { if ($1 > biggest)  biggest=$1; sum += $1; count++ } END { print sum/count " " biggest }' $1
sleep 1;
else
    echo "File" $1 "does not exist"
fi
