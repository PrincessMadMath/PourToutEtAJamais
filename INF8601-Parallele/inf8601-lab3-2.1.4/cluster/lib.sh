# this file must be sourced

if [ -z "$PBS_O_WORKDIR" ]; then
	DIR=$(readlink -f $0)
	PBS_O_WORKDIR=$(dirname $DIR)
	PBS_JOBID="000"
fi

RESULTS_DIR=$PBS_O_WORKDIR/../results
IMAGES="earth-small earth-medium earth-large earth-xlarge"
ITERATIONS="1 2000"
REPEAT=3
OUTPUT_DIR=/tmp/$USER/images/
EXE=$PBS_O_WORKDIR/../src/heatsim

mkdir -p ${RESULTS_DIR}
mkdir -p ${OUTPUT_DIR}

do_run() {
	NP=$1
	DIMX=$2
	DIMY=$3
	ITER="${4:-$ITERATIONS}"
	IMGS="${5:-$IMAGES}"
	TMPOUT=$(mktemp)
	OUT=${RESULTS_DIR}/heatsim.stats
	for ITER in $ITER; do
	for IMG in $IMGS; do
		INPUT=$PBS_O_WORKDIR/../images/$IMG.png
		OUTPUT=${OUTPUT_DIR}/${IMG}_${NP}_${DIMX}_${DIMY}_${ITER}_${PBS_JOBID}.png
		cmd="mpiexec -hostfile $PBS_NODEFILE -np $NP $EXE --dimx $DIMX --dimy $DIMY --iter $ITER --input $INPUT --output $OUTPUT"
		for i in $(seq 1 $REPEAT); do
			echo "Executing: $cmd"
			/usr/bin/time -a -o $TMPOUT -f "$IMG;$NP;$DIMX;$DIMY;$ITER;$i;%e" $cmd
			flock -w 10 $OUT -c "cat $TMPOUT >> $OUT"
		done
	done
	done
}

