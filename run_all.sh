#!/bin/bash

for j in $1 $2 $3 $4
do
	for i in 1 2 3 4 5
	do
		inp="$HOME/OS_project1/OS_PJ1_Test/${j}_${i}.txt"
		outstd="$HOME/OS_project1/output/${j}_${i}_stdout.txt"
		outdmesg="$HOME/OS_project1/output/${j}_${i}_dmesg.txt"
		echo $j $i
		dmesg -c > /tmp/tmp.txt
		./a.out < $inp > $outstd
		dmesg | grep Project1 > $outdmesg
	done
done


inp="$HOME/OS_project1/OS_PJ1_Test/TIME_MEASUREMENT.txt"
outstd="$HOME/OS_project1/output/TIME_MEASUREMENT_stdout.txt"
outdmesg="$HOME/OS_project1/output/TIME_MEASUREMENT_dmesg.txt"
dmesg -c > /tmp/tmp.txt
./a.out < $inp > $outstd
dmesg | grep Project1 > $outdmesg
