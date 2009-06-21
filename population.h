/*****************************************************************************
 * population.h: Header for population.c.
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
#pragma once
#include "strategy.h"

typedef struct {
	int       cstg;   /* How many strategies are currently used [0,cstg) */
	int       maxstg; /* Maximum rgstg array length */
	STRATEGY* rgstg;  /* Array of strategies (chromosomes) */
} POPULATION; /* Pop */

/* Function prototypes */
POPULATION* PopulationCreate(int cStrategies);
void        PopulationRandomize(POPULATION* pPop);
void        PopulationDestroy(POPULATION* pPop);
void        PopulationEmpty(POPULATION* pPop);
bool        PopulationIsFull(POPULATION* pPop);
bool        PopulationAddStrategy(POPULATION* pPop, const STRATEGY* pstgAdd);
void        PopulationSortByFitness(POPULATION* pPop);
