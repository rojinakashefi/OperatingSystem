#!/bin/bash
if [ $# -ne 2 ]; then
	echo "invalid number of input"
	exit 1
fi

for i in "${@}"; do
	if [[ ! $i =~ ^[0-9]+$ ]]; then
		echo "$i is invalid value"
		exit 1
	fi
done

if   [ $1 -gt $2 ]
then 
	echo "$1 is greater than $2"
elif [ $1 -lt $2 ]
then
	echo "$2 is greater than $1"
else echo "$2 is euqal with $1"
fi

echo "sum is $(($1 + $2))"

