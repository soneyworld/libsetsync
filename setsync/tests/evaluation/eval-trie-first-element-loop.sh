#!/bin/bash

BUFFERSIZE=40
	for ((error=1;error < 10; error+=1 ))
	do
#		echo bash eval-trie-buffer.sh $error $1 $BUFFERSIZE
	   bash eval-trie-first-element.sh $error sha1 $BUFFERSIZE
	done

	for ((error=10;error <= 100; error+=10 ))
	do
#		echo bash eval-trie-buffer.sh $error $1 $BUFFERSIZE
	   bash eval-trie-first-element.sh $error sha1 $BUFFERSIZE
	done

