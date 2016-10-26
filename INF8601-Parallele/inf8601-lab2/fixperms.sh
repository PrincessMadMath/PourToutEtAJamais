#!/bin/sh

FILES="configure tests/test-all.sh perf-encode.sh preprocess.py"

for f in $FILES; do
	chmod +x $f
done
