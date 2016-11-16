#!/bin/bash
#
#These commands set up the Grid Environment for your job:
#PBS -N job-heatsim-4
#PBS -q recherche
#PBS -j oe
#PBS -l nodes=4:ppn=8

source $PBS_O_WORKDIR/lib.sh

do_run 32 8 4 "$ITERATIONS" "$IMAGES"

