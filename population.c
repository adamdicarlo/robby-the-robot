/*****************************************************************************
 * population.c: Implementation of POPULATION data structure functions.
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
#include <stdlib.h>
#include "types.h"
#include "error.h"
#include "population.h"


/* Allocates a new POPULATION. */
POPULATION* PopulationCreate(int cStrategies) {
	ASSERT(cStrategies > 0);
	POPULATION* pPop = (POPULATION*) malloc(sizeof(POPULATION));
	VerifyAlloc(pPop, "population");
	pPop->cstg   = cStrategies;
	pPop->maxstg = cStrategies;
	pPop->rgstg  = (STRATEGY*) malloc(sizeof(STRATEGY) * cStrategies);
	VerifyAlloc(pPop, "population strategies");
	return pPop;
}


/* Destroys a POPULATION previously allocated by Population_Create. */
void PopulationDestroy(POPULATION* pPop) {
	ASSERT(pPop);
	free(pPop->rgstg);
	free(pPop);
}


void PopulationRandomize(POPULATION* pPop) {
	ASSERT(pPop);
	int i;
	for (i = 0; i < pPop->cstg; ++i) {
		StrategyRandomize(&pPop->rgstg[i]);
	}
}


/*
 * Empties this population of strategies
 */
void PopulationEmpty(POPULATION* pPop) {
	ASSERT(pPop && pPop->maxstg > 0);
	pPop->cstg = 0;
}


/* Returns: boolean whether population has as many strategies as it can hold */
bool PopulationIsFull(POPULATION* pPop) {
	ASSERT(pPop && pPop->maxstg > 0);
	ASSERT(pPop->cstg <= pPop->maxstg);
	return pPop->cstg >= pPop->maxstg;
}


/*
 * Tries to add a strategy to a population.
 * Returns: boolean success (false if the strategy's full)
 */
bool PopulationAddStrategy(POPULATION* pPop, const STRATEGY* pstgAdd) {
	ASSERT(pPop && pstgAdd);
	if (pPop->cstg >= pPop->maxstg)
		return false;
	
	StrategyCopy(pstgAdd, &pPop->rgstg[pPop->cstg]);
	pPop->cstg++;
	return true;
}


/* Callback comparison function for qsort */
int CompareStrategies(const void* pv1, const void* pv2) {
	STRATEGY* pstg1 = (STRATEGY*) pv1;
	STRATEGY* pstg2 = (STRATEGY*) pv2;

	return (int) (pstg2->rFitness - pstg1->rFitness);
}


/* Sort strategies in a population by fitness */
void PopulationSortByFitness(POPULATION* pPop) {
	ASSERT(pPop);
	ASSERT(pPop->cstg > 0);
	qsort(pPop->rgstg, pPop->cstg, sizeof(pPop->rgstg[0]), CompareStrategies);
}

