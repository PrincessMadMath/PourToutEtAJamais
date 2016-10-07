#!/bin/sh

export OMP_WAIT_POLICY="PASSIVE"
export LC_ALL="C"
THRD="1 2"
HYPERTHREAD="0 1"
FUNC="fast slow_a slow_b slow_c slow_d slow_e slow_f"
COUNTERS="-e cycles -e instructions -e L1-dcache-loads -e L1-dcache-load-misses"
WIDTH=100
HEIGHT=100000
REPEAT=25
OUT=results/perf-$$.out
mkdir -p results/

trap '{ echo "Abort..."; rm -f $OUT rm -f results/stats*; exit 1; }' INT

pmu_benchmark() {
  cat /dev/null > $OUT
    for HT in $HYPERTHREAD; do
    for T in $THRD; do
      if [ $T -eq 1 -a $HT -eq 1 ]; then
        continue;
      fi
      for F in $FUNC; do
      echo "thread=$T,func=$F,ht=$HT" >> $OUT
      perf stat $COUNTERS -o $OUT --append -x "," \
        ./encode/encode --cmd benchmark --thread $T \
        --func $F --width $WIDTH --height $HEIGHT \
        --repeat $REPEAT --hyperthread $HT
      done
    done
  done
  # Remove empty lines if any
  cat $OUT | grep -v -E "^$" > /tmp/perf-$$.out
  mv /tmp/perf-$$.out $OUT
}

time_benchmark() {
  for HT in $HYPERTHREAD; do
    for T in $THRD; do
      if [ $T -eq 1 -a $HT -eq 1 ]; then
        continue;
      fi
      echo "benchmark threads=$T HT=$HT"
      ./encode/encode --cmd benchmark --thread $T --output results/stats-$T-HT$HT.csv \
      --width $WIDTH --height $HEIGHT --repeat $REPEAT --hyperthread $HT
    done
  done
}

case $1 in
  pmu)
    pmu_benchmark
    ./preprocess.py
    ;;
  time)
    time_benchmark
    ;;
  *)
    echo "Please specify a command [ pmu | time ]"
  exit 1
esac



