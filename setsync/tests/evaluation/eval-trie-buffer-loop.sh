#!/bin/bash

#ARG1 = crypto {md5|sha1}

for ((x=1; x <= 10; x +=1 )) 
do
	let BUFFERSIZE=20*2**x
	for ((error=1;error < 10; error+=1 ))
	do
#		echo bash eval-trie-buffer.sh $error $1 $BUFFERSIZE
	   bash eval-trie-buffer.sh $error $1 $BUFFERSIZE
	done

	for ((error=10;error <= 100; error+=10 ))
	do
#		echo bash eval-trie-buffer.sh $error $1 $BUFFERSIZE
	   bash eval-trie-buffer.sh $error $1 $BUFFERSIZE
	done
done
