/*****************************************************************************
 * world.h: Header for world.c.
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

typedef uint8_t CELL;
#define CELL_OPEN	0
#define CELL_CAN	1
#define CELL_WALL	2
#define CELL_INVALID	3
#define ASSERT_CELL(cell)	ASSERT((cell) != CELL_INVALID && (cell) < 4)


typedef struct {
	uint  cx;
	uint  cy;
	uint  xRobby;
	uint  yRobby;
	CELL* cells;
} WORLD; /* wld */


/*
 * STATE represents Robby's view the world from a particular position within it.
 * It tells what is in the current cell and its adjacents cells, that is, the
 * cells directly above, to the left, to the right, and below.
 */
typedef struct {
	unsigned current:2; /* Each is a CELL, as CELLs fit in 2 bits */
	unsigned north:2;
	unsigned south:2;
	unsigned east:2;
	unsigned west:2;
	unsigned index:8;   /* Index into actions table */
} STATE;

/* Function prototypes */
WORLD* WorldCreate(uint cx, uint cy);
WORLD* WorldCreateFromFile(PCSZ pszFilename);
void   WorldDestroy(WORLD* pwld);
void   WorldDump(WORLD* pwld, FILE* out);
void   WorldCopy(WORLD const* pwldSource, WORLD* pwldTarget);
void   WorldSetCansRandomly(WORLD* pwld, double rProbability);
CELL   WorldGetCell(WORLD* pwld, int x, int y);
void   WorldSetCell(WORLD* pwld, int x, int y, CELL cell);
STATE  WorldGetState(WORLD* pwld, int x, int y);
STATE  WorldGetStateFromIndex(int index);
