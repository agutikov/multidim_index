#!/bin/bash

./gen 1 2 > 1_2.csv && ./closest_simple 100_2.csv 1_2.csv && ./closest_graph_index 100_2.csv 1_2.csv 2>path.csv

gnuplot -p -e 'set size ratio 1; plot "100_2.csv" with points, "1_2.csv" with points pointtype 7 ps 1 lc rgb "red", "path.csv" with linespoints ls 1'
