#!/bin/bash

MIN=900
MAX=1000
MIN_FALSE_POSITIVE=0.001
MAX_FALSE_POSITIVE=0.1
STEPSIZE_FALSE_POSITIVE=0.001

./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0001 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0002 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0003 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0004 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0005 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0006 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0007 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0008 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0009 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0011 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0012 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0013 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0014 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0015 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0016 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0017 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0018 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0019 -t BF

./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0021 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0022 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0023 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0024 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0025 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0026 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0027 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0028 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0029 -t BF

./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0031 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0032 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0033 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0034 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0035 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0036 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0037 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0038 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0039 -t BF

./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0041 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0042 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0043 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0044 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0045 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0046 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0047 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0048 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0049 -t BF

./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0051 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0052 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0053 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0054 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0055 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0056 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0057 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0058 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0059 -t BF

./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0061 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0062 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0063 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0064 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0065 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0066 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0067 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0068 -t BF
./ssync -s memDB A $MAX B $MIN -e $MIN --md5 --bf 0.0069 -t BF


exit
COUNTER=$MIN
while [ "$COUNTER" != "$MAX" ]; do
	./ssync -s memDB A $MAX B $COUNTER -e $COUNTER --sha1 --bf 0.1
	let COUNTER=COUNTER+1
done
COUNTER=$MIN
while [ "$COUNTER" != "$MAX" ]; do
	./ssync -s memDB A $MAX B $COUNTER -e $COUNTER --md5 --bf 0.1
	let COUNTER=COUNTER+1
done
