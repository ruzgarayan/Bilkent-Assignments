#!/bin/bash

SIZES="1000 10000 100000 1000000"

make

echo -e "\nExperiment results\n"
for size in $SIZES
do
	echo "_____________________________________"
	echo -e "Number of experiments: ${size}, Serial:"
	./main-serial ${size}
	echo -e "\nParallel:"
	mpirun -np 4 ./main-parallel ${size}
done

wait
#done
