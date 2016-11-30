#!/bin/bash
# lance toutes les jobs

qsub heatsim-1.sh 
qsub heatsim-2.sh 
qsub heatsim-4.sh 
qsub heatsim-8.sh 
qsub heatsim-12.sh
qsub cart-12.sh
