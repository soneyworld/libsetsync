#!/bin/bash

_PATH="eval-bf-p-10000-md5"
mkdir $_PATH

MIN_P=.0001
MAX_P=.1
A=10000
B=0

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
	COUNTER=$MIN_P
	while  float_cond "$COUNTER <= $MAX_P"; do
		FILE="${_PATH}/a_${A}b_${B}equal_${B}p_${COUNTER}_MD5_BF_salt_${SALT}.csv"
		./ssync -s memDB A $A B $B -e $B --salt $SALT --type BF --bf $COUNTER --md5 > $FILE
		echo "$FILE"
		COUNTER=$(float_eval "$COUNTER + $MIN_P")
	done
	let SALT=SALT+1
done
