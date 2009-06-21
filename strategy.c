/*****************************************************************************
 * strategy.c: Functions related to STRATEGY data structure.
 *
 * Copyright (C) 2009  Adam J. DiCarlo <adam.dicarlo@gmail.com>
 *
 * This file is part of Robby the Robot.
 *
 * Robby the Robot is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Robby the Robot is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/
#include <stdlib.h> /* rand */
#include <string.h> /* memcpy */
#include "error.h"
#include "types.h"
#include "strategy.h"


/* Copies a strategy to another already allocated strategy */
void StrategyCopy(const STRATEGY* pstgSource, STRATEGY* pstgTarget) {
	ASSERT(pstgSource && pstgTarget);
	memcpy(pstgTarget, pstgSource, sizeof(STRATEGY));
}


/* Sets this all of a Strategy's actions to random ones */
void StrategyRandomize(STRATEGY* pstg) {
	ASSERT(pstg);
	int i;
	for (i = 0; i < STRATEGY_LENGTH; ++i) {
		pstg->rgact[i] = rand() % NUM_ACTIONS;
	}
}
