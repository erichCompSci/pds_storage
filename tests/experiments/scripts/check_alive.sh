#!/bin/bash

NOISE=5
LOTTA_NOISE=9

for (( i = 1; i <= 10; i++ ))
do
  temp=$(ssh elohrman@windu$i.cc.gatech.edu "pgrep client_modify > temp.txt; wc -l temp.txt | awk '{print \$1}'; rm -f temp.txt")
  temp2=$(ssh elohrman@windu$i.cc.gatech.edu "pgrep client_watch > temp.txt; wc -l temp.txt | awk '{print \$1}'; rm -f temp.txt")
  num_cpus=$(ssh elohrman@windu$i.cc.gatech.edu "nproc")
  both=$[ $temp + $temp2 ]
  if [ $both -ne $num_cpus ]
  then
    echo "There are an incorrect number of processes on windu$i! Client modify: $temp and Client watch: $temp2 Should be: $num_cpus"
  fi
done
