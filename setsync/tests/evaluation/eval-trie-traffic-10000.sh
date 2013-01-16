#!/bin/bash

_PATH="eval-trie-traffic-10000"
mkdir $_PATH

MIN_A=9500
MAX_A=10000
B=10000

MINSALT=0
MAXSALT=10

SALT=$MINSALT

## 10 Rounds
while [ "$SALT" != "$MAXSALT" ]; do
	COUNTER=$MIN_A
	while [ "$COUNTER" != "$MAX_A" ]; do
		FILE="${_PATH}/a_${COUNTER}b_${B}equal_${COUNTER}_TRIE_salt_${SALT}.csv"
		./ssync -s memDB A $COUNTER B $B -e $COUNTER --salt $SALT --type TRIE > $FILE
		echo "$FILE"
		let COUNTER=COUNTER+1
	done
	let SALT=SALT+1
done
