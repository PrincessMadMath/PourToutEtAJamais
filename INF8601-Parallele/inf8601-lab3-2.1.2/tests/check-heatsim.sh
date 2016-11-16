#!/bin/bash
# verifie si heatsim s'execute correctement pour les entrees du banc de test

do_run() {
	cmd="mpirun -np $1 ../src/heatsim --input simple.png --dimx $2 --dimy $3 --iter 100 --output simple_$1_$2_$3.png"
	$cmd
	RET=$?
	if [ $RET -ne 0 ]; then
		echo "error $cmd"
		exit 1
	else
		echo "success $cmd"
	fi
}

do_run 1 1 1
do_run 2 2 1
do_run 2 1 2
do_run 3 3 1
do_run 3 1 3
do_run 4 4 1
do_run 4 1 4
do_run 4 2 2
do_run 5 1 5
do_run 5 5 1
do_run 6 1 6
do_run 6 6 1
do_run 6 2 3
do_run 6 3 2
