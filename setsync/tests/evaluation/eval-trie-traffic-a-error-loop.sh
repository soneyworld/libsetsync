#!/bin/bash

for ((error=0;error < 10; error+=1 ))
do
   bash eval-trie-traffic-a-error.sh $error 100 10000 100 &
done
