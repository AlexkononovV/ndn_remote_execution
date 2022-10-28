#!/bin/sh
#python3 avr_temp.py $1 $2


errfile="$2_err.txt"

python3 avr_temp.py $1 $2 2> "$errfile"; [ -s "$errfile" ] || rm -f "$errfile"
