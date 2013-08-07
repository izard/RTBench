#!/bin/bash
# FIXME AK to refactor from write once mode

if [ $# -ne 4 ]
then
    echo "Usage: ./runBaremetalTest N test signal disturber"
    exit -1
fi

$4 >/dev/null 2>&1 &

NAME=`mktemp`
cd ../../tools/src/baremetal/smallos/linux_boot;
yes|./bootloader smallOS.bin 2 7 >/dev/null 2>&1;
cd ../../phymem;
for i in `seq 1 10`; do
 ./phymem.x -p 0x19000028 -w -l 4 -d 0 >/dev/null 2>&1;
 ./phymem.x -p 0x19000000 -w -l 4 -d $3 >/dev/null 2>&1;
 sleep 2;
 yes|./phymem.x -p 0x19000028 -l 4 | grep "19000028:" | awk -F : ' { print  $2  } ' | sed 's/ //' | awk ' { print strtonum ( $1 ) }'>> $NAME;
done

echo -n "Test:" $2 ":" 
echo `../../../bin/results.sh $NAME`

killall -9 $3

