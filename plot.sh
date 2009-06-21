#!/bin/bash
# plot.sh: Bash script to generate graphs of experiment results using gnuplot.
#
# Copyright (C) 2009  Adam J. DiCarlo <adam.dicarlo@gmail.com>
#
# This file is part of Robby the Robot.
#
# Robby the Robot is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Robby the Robot is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
 
if [ -z "$1" ]; then
	echo "Usage: $0 datafile"
	echo "Example: $0 results/1/run1"
	exit 1
fi

if [ -z "$(which gnuplot)" ]; then
	echo "Skipping plot (gnuplot not found)"
	exit 1
fi

PLOTFILE="$1"
PLOTPS="$PLOTFILE.ps"

cat <<EOS | gnuplot -
	set autoscale
	set xlabel "Generation"
	set ylabel "Generalization Score"
	set size 1.0, 0.6
	plot "$PLOTFILE" using 1:2 title "Robby's Performance" with linespoints
	set terminal postscript portrait enhanced mono dashed lw 1 "Helvetica" 14 
	set output "$PLOTPS"
	replot
EOS
