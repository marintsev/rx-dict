#!/bin/bash

#for (( i=0; i<10000; i++ )); do
#	echo "add"
#	echo $i
#	echo $i
#done

WORDS=`cat ./words.txt | grep add | wc | awk '{ print $1 }'`
echo $WORDS
time cat ./words.txt | ./dictionary
echo $WORDS