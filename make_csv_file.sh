#!/bin/bash


if [ $# -ne 2 ]
then
  echo "Usage: $0 first_dir second_dir"
  exit 1
fi

directory1=$1
directory2=$2

files_in_d1=$( ls $1 )

OLDIFS=$IFS
for file in $files_in_d1
do
  if [ -f "$2/$file" ]
  then
   read -d '\t' line other_stuff < "$1/$file"
   echo "$line" 1>>testing.csv
   #awk -f get_cpu_values.gawk "$1/$file" 1>>testing.csv
   #echo "$line" 1>>testing.csv
   #echo "," 1>>testing.csv
   #awk -f get_cpu_values.gawk "$2/$file" 1>>testing.csv
  else
    echo "$file DOES NOT EXIST IN BOTH DIRECTORIES"
  fi
done

