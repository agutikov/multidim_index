#!/bin/bash

COUNT=100
DIMS=2

./gen $COUNT $DIMS > points.csv
sleep 1
./gen 1 $DIMS > point.csv
./closest_simple points.csv point.csv
./closest_graph_index points.csv point.csv 2>path.csv

gnuplot -p -e 'set size ratio 1; plot "points.csv" with points, "point.csv" with points pointtype 7 ps 1 lc rgb "red", "path.csv" with linespoints ls 1'
