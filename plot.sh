#!/bin/bash

for file in "$1"*.dat; do
    python plot.py $file $2
done
