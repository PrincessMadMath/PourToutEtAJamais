#!/bin/sh

FILES="configure tests/test-all.sh performance.sh preprocess.py trace-dragon"

for f in $FILES; do
	chmod +x $f
done
