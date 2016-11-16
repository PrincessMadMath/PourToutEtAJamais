#!/bin/bash
#
#These commands set up the Grid Environment for your job:
#PBS -N job-heatsim-cart
#PBS -q recherche
#PBS -j oe
#PBS -l nodes=16:ppn=8

source $PBS_O_WORKDIR/lib.sh

do_run 128 1 128
do_run 128 128 1

