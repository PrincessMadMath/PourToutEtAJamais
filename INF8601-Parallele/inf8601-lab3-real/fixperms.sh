#!/bin/sh
FILES="configure cluster/*.sh preprocess.py"

for F in $FILES; do
	chmod +x $F
done
