/*****************************************************************************
 * world.c: Implementation functions dealing with Robby's world.
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
#include <string.h> /* memcpy */
#include "types.h"
#include "error.h"
#include "parse.h"
#include "strategy.h"
#include "world.h"


/* Allocates a new WORLD of given size. Does not set contents. */
WORLD* WorldCreate(uint cx, uint cy) {
	ASSERT(cx > 0 && cy > 0);
	WORLD* pwld = (WORLD*) malloc(sizeof(WORLD));
	VerifyAlloc(pwld, "world");
	pwld->cx = cx;
	pwld->cy = cy;
	pwld->cells = (CELL*) malloc(sizeof(CELL) * cx * cy);
	VerifyAlloc(pwld->cells, "world cells (%dx%d)", cx, cy);
	return pwld;
}


/* Loads a world from a text file */
WORLD* WorldCreateFromFile(PCSZ pszFilename) {
	ASSERT(pszFilename);

	FILE* pf = fopen(pszFilename, "r");
	if (!pf)
		Die("Cannot open '%s'", pszFilename);

	unsigned int cx, cy;
	const int MAX_LINE_LENGTH = 4096;
	char szLine[MAX_LINE_LENGTH + 1];
	char const* pszCur = szLine;
	WORLD* pwld;

	/* read first line (dimensions of map) */
	if (fgets(szLine, MAX_LINE_LENGTH, pf) != szLine)
		Die("Error reading first line from '%s' -- file too short?", pszFilename);
	if (!(ParseUInt(&pszCur, &cx) && ParseUInt(&pszCur, &cy)))
		Die("Malformed first line in '%s'", pszFilename);
	if (cx > MAX_LINE_LENGTH || cy > MAX_LINE_LENGTH)
		Die("World too big (maximum dimensions: %d square)", MAX_LINE_LENGTH);

	pwld = WorldCreate(cx, cy);
	/* Debug("Creating world %d x %d", cx, cy); */

	int x, y;
	bool bGotRobby = false;
	for (y = 0; y < pwld->cy; ++y) {
		if (fgets(szLine, MAX_LINE_LENGTH, pf) != szLine)
			Die("Error reading from '%s' -- file too short?", pszFilename);
		pszCur = szLine;
		for (x = 0; x < pwld->cx; ++x) {
			CELL cell;
			switch (*pszCur) {
			case ' ': cell = CELL_OPEN; break;
			case 'x': cell = CELL_WALL; break;
			case 'R':
				cell = CELL_OPEN;
				pwld->xRobby = x;
				pwld->yRobby = y;
				bGotRobby = true;
				break;
			default:
				cell = CELL_OPEN; /* Silence "uninitialized" warning */
				Die("%s (%d,%d): Unknown character '%c' in world",
					pszFilename, y + 1, x, *pszCur);
			}
			WorldSetCell(pwld, x, y, cell);
			++pszCur;
		}
	}
	fclose(pf);
	if (!bGotRobby)
		Die("World %s contains no Robby start position (R) cell", pszFilename);
	return pwld;
}


/* Deallocates a WORLD allocated with World_Create. */
void WorldDestroy(WORLD* pwld) {
	ASSERT(pwld);
	free(pwld->cells);
	free(pwld);
}


/* Helper for WorldDump */
char CellToChar(CELL c) {
	switch (c) {
	case CELL_OPEN:	return ' ';
	case CELL_WALL: return 'x';
	case CELL_CAN:  return 'c';
	default: break;
	}
	return '?';
}


/* Dumps a world */
void WorldDump(WORLD* pwld, FILE* out) {
	ASSERT(pwld);
	fprintf(out, "**WorldDump**\nSize: %dx%d\n", pwld->cx, pwld->cy);
	fprintf(out, "Robby: (%d, %d)\n", pwld->xRobby, pwld->yRobby);
	int x, y;
	for (y = 0; y < pwld->cy; ++y) {
		for (x = 0; x < pwld->cx; ++x) {
			fprintf(out, "%c", CellToChar(WorldGetCell(pwld, x, y)));
		}
		fprintf(out, "\n");
	}
}


/* Copies the contents of first world to the second */
void WorldCopy(WORLD const* pwldSource, WORLD* pwldTarget) {
	ASSERT(pwldSource && pwldTarget);
	ASSERT(pwldSource->cx == pwldTarget->cx);
	ASSERT(pwldSource->cy == pwldTarget->cy);
	ASSERT(pwldSource->xRobby < pwldSource->cx);
	ASSERT(pwldSource->yRobby < pwldSource->cy);
	ASSERT(pwldSource != pwldTarget);
	ASSERT(pwldSource->cells != pwldTarget->cells);

	int cCells = pwldTarget->cx * pwldTarget->cy;
	pwldTarget->xRobby = pwldSource->xRobby;
	pwldTarget->yRobby = pwldSource->yRobby;
	memcpy(pwldTarget->cells, pwldSource->cells, sizeof(CELL) * cCells);
}


/* Sets cans randomly in open spots in a world.
 * rProbability: Chance (0-1) of a can in each position (used to
 *               the hundredths of a percent) */
void WorldSetCansRandomly(WORLD* pwld, double rProbability) {
	ASSERT(pwld);
	ASSERT(rProbability >= 0.0 && rProbability <= 1.0);

	int i, max = pwld->cx * pwld->cy;
	double rChance = rProbability * 10000.0;
	CELL* pcell = pwld->cells;
	for (i = 0; i < max; ++i) {
		if (*pcell == CELL_OPEN) {
			double rMaybe = (double)(rand() % 10000);
			if (rMaybe < rChance)
				*pcell = CELL_CAN;
		}
		pcell++;
	}
}


/* Returns cell at given coordinates */
CELL WorldGetCell(WORLD* pwld, int x, int y) {
	ASSERT(pwld);
	ASSERT(x >= 0 && y >= 0);
	ASSERT(x < pwld->cx && y < pwld->cy);
	ASSERT_CELL(pwld->cells[y * pwld->cx + x]);
	return pwld->cells[y * pwld->cx + x];
}


/* Sets the cell at given coordinates */
void WorldSetCell(WORLD* pwld, int x, int y, CELL cell) {
	ASSERT(pwld);
	ASSERT(x >= 0 && y >= 0);
	ASSERT(x < pwld->cx && y < pwld->cy);
	ASSERT_CELL(cell);
	pwld->cells[y * pwld->cx + x] = cell;
}


/* Gets STATE for the cell at given coordinates */
STATE WorldGetState(WORLD* pwld, int x, int y) {
	ASSERT(pwld);
	/* coordinates must not be on an edge */
	ASSERT(x >= 1 && y >= 1);
	ASSERT(x < (pwld->cx - 1) && y < (pwld->cy - 1));

	STATE s;
	s.current = WorldGetCell(pwld, x, y);
	s.north   = WorldGetCell(pwld, x, y - 1);
	s.south   = WorldGetCell(pwld, x, y + 1);
	s.west    = WorldGetCell(pwld, x - 1, y);
	s.east    = WorldGetCell(pwld, x + 1, y);
	ASSERT(s.current != CELL_INVALID &&
           s.north   != CELL_INVALID &&
           s.south   != CELL_INVALID &&
           s.east    != CELL_INVALID &&
           s.west    != CELL_INVALID);

	/* Treat STATE values as place-values for a base-3 number so as to
	 * generate contiguous indices with all possible STATE combinations. */
	unsigned int index =
		s.east  * 81 +
		s.west  * 27 +
		s.south *  9 +
		s.north *  3 +
		s.current;
	ASSERT(index < STRATEGY_LENGTH);
	s.index = index;
	return s;
}


/* Translates an action index into a State value */
STATE WorldGetStateFromIndex(int index) {
	ASSERT(index >= 0 && index < STRATEGY_LENGTH);

	STATE s;
	s.east    = (index / 81) % 3;
	s.west    = (index / 27) % 3;
	s.south   = (index /  9) % 3;
	s.north   = (index /  3) % 3;
	s.current =  index % 3;
	s.index = index;
	ASSERT(s.current != CELL_INVALID &&
           s.north   != CELL_INVALID &&
           s.south   != CELL_INVALID &&
           s.east    != CELL_INVALID &&
           s.west    != CELL_INVALID);
#ifndef NDEBUG
	unsigned int check =
		s.east  * 81 +
		s.west  * 27 +
		s.south *  9 +
		s.north *  3 +
		s.current;
	ASSERT(check == index);
#endif
	return s;
}
