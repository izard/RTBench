#!/bin/bash
# FIXME AK to refactor from write once mode
# Runs baremetal test in 3 threads

if [ $# -ne 4 ]
then
    echo "Usage: ./runBaremetalTest N test signal disturber"
    exit -1
fi

$4 >/dev/null 2>&1 &

NAME=`mktemp`
cd ../../tools/src/baremetal/smallos/linux_boot
for i in `seq 1 10`; do 
 yes|./bootloader smallOS.bin 2 7 >/dev/null 2>&1
 sleep 1;
 yes|./bootloader smallOS4.bin 4 9 >/dev/null 2>&1
 sleep 1;
 yes|./bootloader smallOS6.bin 6 b >/dev/null 2>&1
 sleep 1;
 cd ../../phymem;
 ./phymem.x -p 0x19000028 -w -l 4 -d 0 >/dev/null 2>&1;
 ./phymem.x -p 0x19000100 -w -l 4 -d 0 >/dev/null 2>&1;
 ./phymem.x -p 0x19000000 -w -l 4 -d 26 >/dev/null 2>&1;
 sleep 1;
 ./phymem.x -p 0x19000040 -w -l 4 -d 27 >/dev/null 2>&1;
 sleep 1;
 ./phymem.x -p 0x19000080 -w -l 4 -d 28 >/dev/null 2>&1;
 sleep 3;
 yes|./phymem.x -p 0x19000028 -l 4 | grep "19000028:" | awk -F : ' { print  $2  } ' | sed 's/ //' | awk ' { print strtonum ( $1 ) }'>> $NAME;
 cd ../smallos/linux_boot
done
echo -n "Test:" $2 ":" 
echo `../../../../bin/results.sh $NAME`

killall -9 $3
