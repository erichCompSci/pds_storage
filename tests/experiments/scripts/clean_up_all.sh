#!/bin/bash

for (( i = 1; i <= 10; i++ ))
do
  ssh elohrman@windu$i.cc.gatech.edu "pkill pdsd; pkill client_modify; pkill client_watch;"
done

echo CLEANED!
