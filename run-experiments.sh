#!/bin/bash
# run-experiments.sh: Bash script to run several experiments, record outputs,
# generate graphs using gnuplot, and display basic statistics.
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
 
RUNS_PER_EXPERIMENT=10

# Mac OS X doesn't provide "seq," so reimplement it
seq() {
	local MIN=$1
	local MAX=$2
	local i
	for ((i=$MIN; $i <= $MAX; i++)); do
		echo "$i"
	done
}

calc () {
	echo "scale=12; $1" | bc
}

# Runs an experiment $RUNS_PER_EXPERIMENT times and plots the best run
run_experiment() {
	local NUMBER="$1"
	shift 1
	local BEST=0
	local BEST_SCORE=-9000
	local RESULTS
	local SCORE
	for run in $(seq 1 $RUNS_PER_EXPERIMENT); do
		echo -n "Experiment $NUMBER, run $run..."
		RESULTS="results/$NUMBER/run$run"
		./robby -r $run $* > "$RESULTS"
		SCORE=$(tail -1 "$RESULTS" | cut -f 4 -d ' ')
		echo " $SCORE"
		if [[ "$(calc "$SCORE > $BEST_SCORE")" == "1" ]]; then
			BEST_SCORE=$SCORE
			BEST=$run
		fi
	done
	echo "Best run: $BEST ($BEST_SCORE)"
	if ./plot.sh results/$NUMBER/run$BEST; then
		mv results/$NUMBER/run$BEST.ps results/exp$NUMBER-$BEST-best.ps
	fi
}

if [[ ! -x ./robby ]]; then
	echo "Please build Robby using the command 'scons' first."
	exit 1
fi

mkdir -p results/{1,2,3,4,5,6} 2>/dev/null

# Experiment 1: Evolve Robby with default parameters
run_experiment 1

# Experiment 2: Evolve Robby without crossover
run_experiment 2 "-x"

# Experiment 3: Increase population size to 500
run_experiment 3 "-p 500"

# Experiment 4: Decrease population size to 100
run_experiment 4 "-p 100"

# Experiment 5: Use SmartRobby
run_experiment 5 "-z smart"

# Experiment 6: Use IntelligentDesignRobby
run_experiment 6 "-z id"
