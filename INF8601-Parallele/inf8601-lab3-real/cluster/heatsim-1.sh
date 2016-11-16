#!/bin/bash
#
#These commands set up the Grid Environment for your job:
#PBS -N job-heatsim-1
#PBS -q recherche
#PBS -j oe
#PBS -l nodes=1:ppn=8

source $PBS_O_WORKDIR/lib.sh

IMAGES="earth-small earth-medium"

# do_run NP DIMX DIMY ITERATIONS IMAGES
do_run 1 1 1 "$ITERATIONS" "$IMAGES"
do_run 2 2 1 "$ITERATIONS" "$IMAGES"
do_run 4 2 2 "$ITERATIONS" "$IMAGES"
do_run 8 4 2 "$ITERATIONS" "$IMAGES"
