#!/bin/bash

EXPER_DIR="${HOME}/Systems/build_pds_storage/tests/experiments/with_Storage"
wait_time=.1

(ssh elohrman@windu1.cc.gatech.edu <<'ENDSSH'
~/Systems/build_pds_storage/server/pdsd -no_fork > /dev/null &
ENDSSH
) &
# This value has to be high to fake avoid a race condition
sleep .75

for (( i = 2; i <= 22; i++ ))
do
  group=$[ $i % 3 + 1 ]
  ssh elohrman@windu1.cc.gatech.edu "$EXPER_DIR/client_watch_stor $i $group </dev/null >/dev/null &" &
  sleep $wait_time 
done

for (( i = 2; i <= 10; i++ ))
do
  num_cpus=$(ssh elohrman@windu$i.cc.gatech.edu "nproc")
  for (( j = 2; j <= $num_cpus; j++ ))
  do
    group=$[ $j % 3 + 1 ] 
    ssh elohrman@windu$i.cc.gatech.edu "$EXPER_DIR/client_watch_stor $j $group </dev/null >/dev/null &" &
    sleep $wait_time 
  done
done

for (( i = 1; i <= 10; i++ ))
do
  modify_id=$[ ( $i - 1 ) / 3 ]
  group_id=$[ $i % 3 + 1 ]
  ssh elohrman@windu$i.cc.gatech.edu "$EXPER_DIR/client_modify_stor $modify_id $group_id </dev/null >/dev/null &" &
  sleep $wait_time
done

echo FINISHED!

