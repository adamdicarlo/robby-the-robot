/*****************************************************************************
 * misc.c: Miscellaneous support functions.
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
#include <stdlib.h> /* rand() */
#include "error.h"  /* ASSERT() */
#include "misc.h"

/* Swap two pointers */
void SwapPointers(void** ppvLeft, void** ppvRight) {
	ASSERT(ppvLeft && ppvRight && *ppvLeft && *ppvRight);
	void* pvHold;
	pvHold = *ppvLeft;
	*ppvLeft = *ppvRight;
	*ppvRight = pvHold;
}


/* Sum array of ints */
int Sum(const int n[], int c) {
	ASSERT(n);
	ASSERT(n > 0); /* n<0: definite bug; n==0: likely bug */

	int i, nSum = 0;
	for (i = 0; i < c; ++i) {
		nSum += n[i];
	}
	return nSum;
}


/* Grab random number between 0.0 and 1.0 inclusive, at least
 * theoretically (if the RAND_MAX cast is represented accurately
 * in double-precision floating point) */
double RandomZeroOne() {
	return (double)rand() / (double)RAND_MAX;
}
