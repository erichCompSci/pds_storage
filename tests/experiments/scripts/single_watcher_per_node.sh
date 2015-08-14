#!/bin/bash

EXPER_DIR="${HOME}/Systems/build_pds_storage/tests/experiments/wout_Storage"
wait_time=.1

(ssh elohrman@windu1.cc.gatech.edu <<'ENDSSH'
~/Systems/build_pds_storage/server/pdsd -no_fork > /dev/null &
ENDSSH
) &
#This value has to be high to fake avoid a race condition
sleep .75

total_cpus_left=0
#Initialize an array for determing the number left for modifiers
counting_array=(0 0 0 0 0 0 0 0 0 0)

for (( i = 1; i <= 10; i++ ))
do
  group=$[ $i % 3 + 1 ]
  ssh elohrman@windu$i.cc.gatech.edu "$EXPER_DIR/client_watch $i $group </dev/null >/dev/null &" &
  sleep $wait_time 
  num_cpus=$(ssh elohrman@windu$i.cc.gatch.edu "nproc")
  total_cpus_left=$[ $total_cpus_left + $num_cpus - 1]
  temp_index=$[ $i - 1 ]
  counting_array[$temp_index]=$[ $num_cpus - 1 ]
done

#Subtract one for the pdsd server
total_cpus_left=$[ $total_cpus_left - 1 ]
counting_array[0]=$[ ${couting_array[0]} - 1]


#Initialize an array for determing modify_id in group
mod_id_array=(0 0 0)

#I want to evenly place the client modifiers on the nodes so here I go
#NOTE: The nodes are labeled 1 to 10, but the index is labeled 0 to 9...this makes things messy
for (( i = 0; i < $total_cpus_left; i++ ))
do
  index=$[ $i % 10  + 1]
  mod_id_index=$[ $i % 3 + 1]
  mod_id_array_index=$[ $mod_id_index - 1 ]
  mod_id=${mod_id_array[$mod_id_array_index]}
  mod_id_array[$mod_id_array_index]=$[ ${mod_id_array[$mod_id_array_index]} + 1 ]
    
  valid=1
  while [ $valid -ne 0 ]
  do
    array_index=$[ $index - 1 ]
    if [ ${counting_array[$array_index]} -ne 0 ]
    then
      #echo "Placing on: $index"
      #echo "Counting array: ${counting_array[$array_index]}"
      ssh elohrman@windu$index.cc.gatech.edu "$EXPER_DIR/client_modify $mod_id $mod_id_index </dev/null >/dev/null &" &
      if [ $mod_id -eq 0 ]
      then
        sleep 2
      fi
      counting_array[$array_index]=$[ ${counting_array[$array_index]} - 1 ]
      valid=0
      sleep $wait_time
    else
      new_index=$[ $index + 1 ]
      if [ $new_index -eq 11 ]
      then
        index=1
      else
        index=$new_index
      fi
    fi
  done

done

echo "Finished starting the processes..."

sleep 915

. ./clean_up_all.sh

echo FINISHED!

