#!/bin/bash

for (( count = 1; count <= 10; count++ ))
do
temp=$(ssh elohrman@windu$count.cc.gatech.edu "nproc")
if [ $temp -ne 24 ]
then
echo "Windu$count does not have 24 processors, it has: $temp"
fi
done
echo "Finished!"
