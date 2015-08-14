#!/bin/bash

DUMP_DIR=$HOME/Systems/build_pds_storage/dump_dir

if [ $# -ne 1 ]
then
  copy_dir=$HOME
else
  copy_dir=$1
fi

echo "Killing the processes..."

for (( i = 1; i <= 10; i++ ))
do
  ssh elohrman@windu$i.cc.gatech.edu "pkill pdsd; pkill client_modify; pkill client_watch;"
done

if [ "$(ls -A $DUMP_DIR)" ]
then
  echo "Copying the non-empty directory..."
  cp -R $DUMP_DIR $copy_dir
  mv $copy_dir/dump_dir "$copy_dir/exper_$(date +%d%m%Y%R)"
fi

cd $DUMP_DIR
if [ $(pwd) = $DUMP_DIR ]
then
  echo "Cleaning the dump_directory..."
  rm -f *
else
  echo "NOT IN DUMP_DIR, IN: $(pwd)"
fi


echo "CLEANED!"
