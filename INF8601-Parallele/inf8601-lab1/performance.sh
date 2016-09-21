#!/bin/sh
# 
# Script permettant d'obtenir les données de performance de dragonizer
#

# variables
EXE="./src/dragonizer"
LIBS="pthread tbb"
SERIAL="serial"
PWR=28
THREADS_MAX=8
CMDS="draw limits"
REPEAT=3
OUT_DIR="results"
OUT_PRE="time_dragonizer.data"

run_experiment() {

	cmd=$1
	lib=$2
	pwr=$3
	thd=$4
	
	OUT="${OUT_DIR}/${OUT_PRE}"
	PGM="${OUT_DIR}/dragon_${lib}_${pwr}.pgm"
	CMD="$EXE --cmd $cmd --lib $lib --power 1 --max $pwr --thread $thd  -o $PGM"
	touch $OUT
	echo "running cmd=$cmd lib=$lib pwr=$pwr thd=$thd"
	/usr/bin/time -f "$cmd,$lib,$pwr,$thd,%S,%U,%e" -o $OUT -a $CMD
}

mkdir -p $OUT_DIR

run_serial() {
	for cmd in $CMDS; do
	for i in $(seq 1 $REPEAT); do
		run_experiment $cmd $SERIAL $PWR 1
	done
	done
}

run_parallel() {
	for cmd in $CMDS; do
	for lib in $LIBS; do
	for thd in $(seq 1 $THREADS_MAX); do
	for i in $(seq 1 $REPEAT); do
		run_experiment $cmd $lib $PWR $thd
	done
	done
	done
	done
}

case $1 in 
	serial)
		run_serial
		;;
	parallel)
		run_parallel
		;;
	*)
		echo "Unknown or missing parameter [ serial | parallel ]"
		exit 1
esac

exit 0
