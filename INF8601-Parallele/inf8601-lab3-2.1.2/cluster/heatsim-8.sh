#!/bin/bash
#
#These commands set up the Grid Environment for your job:
#PBS -N job-heatsim-8
#PBS -q recherche
#PBS -j oe
#PBS -l nodes=8:ppn=8

source $PBS_O_WORKDIR/lib.sh

do_run 64 8 8 "$ITERATIONS" "$IMAGES"

