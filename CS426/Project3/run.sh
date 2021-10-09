#!/bin/bash

SIZES="100 500 1000 2000 3000"

make

echo -e "Note: I included smaller values of n in this shell script,\nbecause larger values were taking too long time.\n"
echo -e "\nExperiment results\n"
for size in $SIZES
do
	echo -e "\n#####################################"
	echo "n=${size}"
	echo "#####################################"
	echo -e "Serial:"
	./main-serial ${size}
	echo -e "\nParallel:"
	./main-parallel ${size}
done

wait
#done
