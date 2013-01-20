#!/bin/bash

_PATH="eval-bftest-p-100-10000-sha1"
mkdir $_PATH

MIN_A=100
MAX_A=10000
MAX_ERROR=10
STEPSIZE=100

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
	while  float_cond "$COUNTER <= $MAX_A"; do
		P=$(float_eval "$MAX_ERROR / $COUNTER")
		FILE="${_PATH}/a_${COUNTER}p_${P}_salt_${SALT}.csv"
		if [ ! -f $FILE ]; then 
		./bftest A $COUNTER salt $SALT p $P > $FILE
		fi
		echo "$FILE"
		let COUNTER=$COUNTER+$STEPSIZE
	done
	let SALT=SALT+1
done
