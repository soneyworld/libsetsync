#!/bin/bash

_PATH="eval-max-elements"
mkdir $_PATH

A=900
B=1000
E=900

MIN_FALSE_POSITIVE=0.001
MID_FALSE_POSITIVE=0.01
MAX_FALSE_POSITIVE=0.1

MIN=500
MAX=1500

MINSALT=0
MAXSALT=10

SALT=$MINSALT
## 10 Rounds
while [ "$SALT" != "$MAXSALT" ]; do

## Bloom Filter only
	COUNTER=$MIN
	while [ "$COUNTER" != "$MAX" ]; do
		FILE="${_PATH}/a_${A}b_${B}equal_${E}p_${MIN_FALSE_POSITIVE}max_${COUNTER}_BF_salt_${SALT}.csv"
		./ssync -s memDB A $A B $B -e $E --bf $MIN_FALSE_POSITIVE --max $COUNTER --salt $SALT --type BF > $FILE
		echo "$FILE"
		let COUNTER=COUNTER+10
	done

	COUNTER=$MIN
	while [ "$COUNTER" != "$MAX" ]; do
		FILE="${_PATH}/a_${A}b_${B}equal_${E}p_${MID_FALSE_POSITIVE}max_${COUNTER}_BF_salt_${SALT}.csv"
		./ssync -s memDB A $A B $B -e $E --bf $MID_FALSE_POSITIVE --max $COUNTER --salt $SALT --type BF > $FILE
		echo "$FILE"
		let COUNTER=COUNTER+10
	done

	COUNTER=$MIN
	while [ "$COUNTER" != "$MAX" ]; do
		FILE="${_PATH}/a_${A}b_${B}equal_${E}p_${MAX_FALSE_POSITIVE}max_${COUNTER}_BF_salt_${SALT}.csv"
		./ssync -s memDB A $A B $B -e $E --bf $MAX_FALSE_POSITIVE --max $COUNTER --salt $SALT --type BF > $FILE
		echo "$FILE"
		let COUNTER=COUNTER+10
	done

## Bloom Filter and Trie

	COUNTER=$MIN
	while [ "$COUNTER" != "$MAX" ]; do
		FILE="${_PATH}/a_${A}b_${B}equal_${E}p_${MIN_FALSE_POSITIVE}max_${COUNTER}_BOTH_salt_${SALT}.csv"
		./ssync -s memDB A $A B $B -e $E --bf $MIN_FALSE_POSITIVE --max $COUNTER --salt $SALT > $FILE
		echo "$FILE"
		let COUNTER=COUNTER+10
	done

	COUNTER=$MIN
	while [ "$COUNTER" != "$MAX" ]; do
		FILE="${_PATH}/a_${A}b_${B}equal_${E}p_${MID_FALSE_POSITIVE}max_${COUNTER}_BOTH_salt_${SALT}.csv"
		./ssync -s memDB A $A B $B -e $E --bf $MID_FALSE_POSITIVE --max $COUNTER --salt $SALT > $FILE
		echo "$FILE"
		let COUNTER=COUNTER+10
	done

	COUNTER=$MIN
	while [ "$COUNTER" != "$MAX" ]; do
		FILE="${_PATH}/a_${A}b_${B}equal_${E}p_${MAX_FALSE_POSITIVE}max_${COUNTER}_BOTH_salt_${SALT}.csv"
		./ssync -s memDB A $A B $B -e $E --bf $MAX_FALSE_POSITIVE --max $COUNTER --salt $SALT > $FILE
		echo "$FILE"
		let COUNTER=COUNTER+10
	done
	let SALT=SALT+1
done
