#!/bin/bash

if [ $# -ne 1 ]
then
  echo "Usage: $0 directory_to_check"
fi

dir=$1
files=$(ls $1)

for file in $files
do
  empty=$( awk 'END { print NR }' $dir/$file)
  if [ $empty -eq 0 ]
  then 
    echo "FILE: $file IS EMPTY"
  fi
done

echo "FINISHED!"
