#!/bin/bash


if [ $# -ne 3 ]
then
  echo "Usage: $0 first_dir second_dir comp_file_name"
  exit 1
fi

directory1=$1
directory2=$2
dump_file_name=$3

files_in_d1=$( ls $1 )

OLDIFS=$IFS
for file in $files_in_d1
do
  if [ -f "$2/$file" ]
  then
   read -d '\t' line other_stuff < "$1/$file"
   temp=$line"_simple,"
   printf "$temp" 1>>$dump_file_name
   awk -f get_cpu_values.gawk "$1/$file" 1>>$dump_file_name
   temp=$line"_stor,"
   printf "$temp" 1>>$dump_file_name
   awk -f get_cpu_values.gawk "$2/$file" 1>>$dump_file_name
   printf "\n" 1>>$dump_file_name
  else
    echo "$file DOES NOT EXIST IN BOTH DIRECTORIES"
  fi
done

