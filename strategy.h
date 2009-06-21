/*****************************************************************************
 * strategy.h: Header for strategy.c.
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

typedef enum {
	MoveNorth  = 0,
	MoveSouth  = 1,
	MoveEast   = 2,
	MoveWest   = 3,
	MoveRandom = 4,
	StayPut    = 5,
	PickUpCan  = 6
} ACTION;
#define NUM_ACTIONS  7

/* Number of actions in a strategy */
#define STRATEGY_LENGTH  243

/* An individual chromosome in the population, or "strategy" */
typedef struct {
	/* ACTIONs stored as bytes for hopefully better cache performance */
	uint8_t rgact[STRATEGY_LENGTH];
	double  rFitness; /* Average score after 'NUM_SESSIONS' cleanings */
} STRATEGY; /* stg */


/* Function prototypes */
void StrategyCopy(const STRATEGY* pstgSource, STRATEGY* pstgTarget);
void StrategyRandomize(STRATEGY* pstg);
