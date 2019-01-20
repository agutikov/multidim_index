#!/bin/bash

#gnuplot -p -e 'set size ratio 1; plot "xxx.csv" with points, "point.csv" with points pointtype 7 ps 1 lc rgb "red", "path.csv" with linespoints ls 1'
gnuplot -p -e 'set datafile separator ","; set size ratio 1; set xrange [-0.25:1.25]; set yrange [-0.25:1.25]; plot "'$1'" with points, "'$2'"  with circles lc rgb "blue" fs transparent solid 0.05 noborder'
