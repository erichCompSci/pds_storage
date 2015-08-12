#!/bin/bash


if [ $# -ne 2 ]
then
  echo "Usage: $0 first_dir second_dir"
  exit 1
fi

directory1=$1
directory2=$2

files_in_d1=$( ls $1 )

for file in $files_in_d1
do
  if [ -f $2/$file ]
  then
    awk -f get_cpu_values.gawk $1/$file 1>>testing.csv
  else
    echo "$file DOES NOT EXIST IN BOTH DIRECTORIES"
  fi
done

