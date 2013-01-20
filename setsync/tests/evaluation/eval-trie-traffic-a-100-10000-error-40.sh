#!/bin/bash

_PATH="eval-trie-traffic-a-100-10000-error-40"
mkdir $_PATH

MIN_A=100
MAX_A=10000
ERROR=40


MINSALT=0
MAXSALT=10

SALT=$MINSALT

float_scale=8

#####################################################################
# Evaluate a floating point number expression.

function float_eval()
{
    local stat=0
    local result=0.0
    if [[ $# -gt 0 ]]; then
        result=$(echo "scale=$float_scale; $*" | bc -q 2>/dev/null)
        stat=$?
        if [[ $stat -eq 0  &&  -z "$result" ]]; then stat=1; fi
    fi
    echo $result
    return $stat
}

#####################################################################
# Evaluate a floating point number conditional expression.

function float_cond()
{
    local cond=0
    if [[ $# -gt 0 ]]; then
        cond=$(echo "$*" | bc -q 2>/dev/null)
        if [[ -z "$cond" ]]; then cond=0; fi
        if [[ "$cond" != 0  &&  "$cond" != 1 ]]; then cond=0; fi
    fi
    local stat=$((cond == 0))
    return $stat
}

## 10 Rounds
while [ "$SALT" != "$MAXSALT" ]; do
	COUNTER=$MIN_A
	while float_cond "$COUNTER <= $MAX_A"; do
		B=$(float_eval "$COUNTER - $ERROR")
		FILE="${_PATH}/a_${COUNTER}b_${B}equal_${B}_TRIE_salt_${SALT}.csv"
		./ssync -s memDB A $COUNTER B $B -e $B --salt $SALT --type TRIE > $FILE
		echo "$FILE"
		let COUNTER=COUNTER+100
	done
	let SALT=SALT+1
done
