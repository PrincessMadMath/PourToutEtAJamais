#!/bin/bash
#
#These commands set up the Grid Environment for your job:
#PBS -N job-heatsim-2
#PBS -q recherche
#PBS -j oe
#PBS -l nodes=2:ppn=8

source $PBS_O_WORKDIR/lib.sh

do_run 16 4 4 "$ITERATIONS" "$IMAGES"
