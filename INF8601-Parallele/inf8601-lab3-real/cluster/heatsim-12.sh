#!/bin/bash
#
#These commands set up the Grid Environment for your job:
#PBS -N job-heatsim-12
#PBS -q recherche
#PBS -j oe
#PBS -l nodes=12:ppn=8

source $PBS_O_WORKDIR/lib.sh

do_run 96 12 8 "$ITERATIONS" "$IMAGES"

