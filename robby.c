/*****************************************************************************
 * robby.c: Genetic algorithm implementation of Robby, the Soda-Can-Collecting
 * Robot. Robby's actions and scoring.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "error.h"
#include "main.h"
#include "strategy.h"
#include "population.h"
#include "world.h"
#include "robby.h"


/* Action scoring; Robby's punishments and rewards */
#define ROBBY_HIT_WALL_PUNISHMENT     -5
#define ROBBY_PICK_UP_CAN_REWARD      10
#define ROBBY_PICK_UP_CAN_PUNISHMENT  -1


/* Local functions */
static int RobbyMoveNorth(WORLD* pwld, STATE s);
static int RobbyMoveSouth(WORLD* pwld, STATE s);
static int RobbyMoveEast(WORLD* pwld, STATE s);
static int RobbyMoveWest(WORLD* pwld, STATE s);
static int RobbyMoveRandom(WORLD* pwld, STATE s);
static int RobbyStayPut(WORLD* pwld, STATE s);
static int RobbyPickUpCan(WORLD* pwld, STATE s);

#define NUM_SMART_ACTIONS	5
static int SmartRobbyMoveNorth(WORLD* pwld, STATE s);
static int SmartRobbyMoveSouth(WORLD* pwld, STATE s);
static int SmartRobbyMoveEast(WORLD* pwld, STATE s);
static int SmartRobbyMoveWest(WORLD* pwld, STATE s);
static int SmartRobbyMoveRandom(WORLD* pwld, STATE s);



/*
 * Robby's action handlers; one function per action
 */
static int (*k_rgpfnActions[NUM_ACTIONS])(WORLD*, STATE) = {
	RobbyMoveNorth,
	RobbyMoveSouth,
	RobbyMoveEast,
	RobbyMoveWest,
	RobbyMoveRandom,
	RobbyStayPut,
	RobbyPickUpCan
};


static int (*k_rgpfnSmartActions[NUM_SMART_ACTIONS])(WORLD*, STATE) = {
	SmartRobbyMoveNorth,
	SmartRobbyMoveSouth,
	SmartRobbyMoveEast,
	SmartRobbyMoveWest,
	SmartRobbyMoveRandom
};


/* Robby action handlers: Each returns the score of the action performed. */

/* Normal Robby action handlers follow. */
static int RobbyMoveNorth(WORLD* pwld, STATE s) {
	ASSERT(pwld);
	if (s.north == CELL_WALL)
		return ROBBY_HIT_WALL_PUNISHMENT;
	pwld->yRobby--;
	return 0;
}


static int RobbyMoveSouth(WORLD* pwld, STATE s) {
	ASSERT(pwld);
	if (s.south == CELL_WALL)
		return ROBBY_HIT_WALL_PUNISHMENT;
	pwld->yRobby++;
	return 0;
}


static int RobbyMoveEast(WORLD* pwld, STATE s) {
	ASSERT(pwld);
	if (s.east == CELL_WALL)
		return ROBBY_HIT_WALL_PUNISHMENT;
	pwld->xRobby++;
	return 0;
}


static int RobbyMoveWest(WORLD* pwld, STATE s) {
	ASSERT(pwld);
	if (s.west == CELL_WALL)
		return ROBBY_HIT_WALL_PUNISHMENT;
	pwld->xRobby--;
	return 0;
}


static int RobbyMoveRandom(WORLD* pwld, STATE s) {
	ASSERT(pwld);
	return (k_rgpfnActions[rand() % 4])(pwld, s);
}


static int RobbyStayPut(WORLD* pwld, STATE s) {
	ASSERT(pwld);
	return 0;
}


static int RobbyPickUpCan(WORLD* pwld, STATE s) {
	ASSERT(pwld);
	if (s.current == CELL_CAN) {
		/* Remove the can and reward Robby */
		WorldSetCell(pwld, pwld->xRobby, pwld->yRobby, CELL_OPEN);
		return ROBBY_PICK_UP_CAN_REWARD;
	}
	return ROBBY_PICK_UP_CAN_PUNISHMENT;
}


/* SmartRobby handlers follow. SmartRobby never bumps into walls, but instead
 * attempts a clockwise wall-avoidance scheme */

static int SmartRobbyMoveNorth(WORLD* pwld, STATE s) {
	ASSERT(pwld);
	if (s.north == CELL_WALL)
		return SmartRobbyMoveEast(pwld, s);
	pwld->yRobby--;
	return 0;
}


static int SmartRobbyMoveSouth(WORLD* pwld, STATE s) {
	ASSERT(pwld);
	if (s.south == CELL_WALL)
		return SmartRobbyMoveWest(pwld, s);
	pwld->yRobby++;
	return 0;
}


static int SmartRobbyMoveEast(WORLD* pwld, STATE s) {
	ASSERT(pwld);
	if (s.east == CELL_WALL)
		return SmartRobbyMoveSouth(pwld, s);
	pwld->xRobby++;
	return 0;
}


static int SmartRobbyMoveWest(WORLD* pwld, STATE s) {
	ASSERT(pwld);
	if (s.west == CELL_WALL)
		return SmartRobbyMoveNorth(pwld, s);
	pwld->xRobby--;
	return 0;
}


static int SmartRobbyMoveRandom(WORLD* pwld, STATE s) {
	ASSERT(pwld);
	return (k_rgpfnSmartActions[rand() % 4])(pwld, s);
}


/*
 * Runs one Robby cleaning session on the given world (which is changed).
 * Returns Robby's score for this cleaning session.
 */
int RobbyClean(const ARGS* pArgs, WORLD* pwld, STRATEGY* pstg, int cActions) {
	ASSERT(pwld && pstg);
	ASSERT(cActions > 0);

	int nScore = 0;
	int i;
	for (i = 0; i < cActions; i++) {
		STATE s = WorldGetState(pwld, pwld->xRobby, pwld->yRobby);
		ACTION a = pstg->rgact[s.index];
		ASSERT(a >= 0 && a < NUM_ACTIONS);

		/* Dispatch the Robby action to appropriate handler */
		if (pArgs->robbyType == SmartRobby) {
			/* SmartRobby picks up cans whenever possible, ignoring his genes */
			if (s.current == CELL_CAN) {
				nScore += RobbyPickUpCan(pwld, s);
				continue;
			}
			nScore += k_rgpfnSmartActions[a % NUM_SMART_ACTIONS](pwld, s);
		} else {
			nScore += k_rgpfnActions[a](pwld, s);
		}
	}
	return nScore;
}
