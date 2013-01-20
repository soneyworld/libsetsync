#!/bin/bash

# md5 / sha1
HASH=$1

_PATH="eval-bftest-p-small-$HASH"
mkdir $_PATH

MIN_P=.01
MAX_P=.1
MIN_A=1000
MAX_A=10000

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
	A=$MIN_A
	while float_cond "$A <= $MAX_A"; do
		COUNTER=$MIN_P
		while  float_cond "$COUNTER <= $MAX_P"; do
			FILE="${_PATH}/a_${A}p_${COUNTER}_${HASH}_salt_${SALT}.csv"
			if [ ! -f $FILE ]; then 
				./bftest A $A salt $SALT p $COUNTER --$HASH > $FILE
			fi
			echo "$FILE"
			COUNTER=$(float_eval "$COUNTER + $MIN_P")
		done
		let A=$(float_eval "$A + $MIN_A")
	done
	let SALT=SALT+1
done
