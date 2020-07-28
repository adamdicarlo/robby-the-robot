/*****************************************************************************
 * parse.c: Simple token extraction for strings
 *
 * WARNING! This codebase uses an uncommon variable naming convention. It makes
 * sense--with an explanation--I swear! Please see the README.
 *
 * ---------------------------------------------------------------------------
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
#include <assert.h>
#include <ctype.h>  /* isspace... */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h> /* atoi, atof */
#include "parse.h"


/* Update caller's string pointer to point beyond leading whitespace
 * (pure spaces only) */
void SkipWhitespace(char const** ppszLine) {
        assert(ppszLine && *ppszLine);
        while (**ppszLine == ' ')
                ++(*ppszLine);
}


/* 
 * Read a string from an input string into *pszTarget and update caller's
 * input string pointer so it points past the location of the string which
 * was read
 */
bool ParseString(char const** ppszLine, char* pszTarget) {
        assert(ppszLine && *ppszLine);
        assert(pszTarget);

        SkipWhitespace(ppszLine);
        while (**ppszLine != '\0' && !isspace(**ppszLine)) {
                if (!isascii(**ppszLine))
                        return false;
                *pszTarget++ = **ppszLine;
                (*ppszLine)++;
        }
        *pszTarget = '\0';
        return true;
}

bool ParseUInt(char const** ppszLine, unsigned int* pnTarget) {
	assert(ppszLine && *ppszLine);
        assert(pnTarget);
	const int MAX_UINT_STRING = 11;
	char szBuffer[MAX_UINT_STRING + 1];

        SkipWhitespace(ppszLine);
        int len = 0;
        while (**ppszLine != '\0' && !isspace(**ppszLine)) {
                if (!isdigit(**ppszLine) || len >= MAX_UINT_STRING)
                        return false;
                szBuffer[len++] = **ppszLine;
                (*ppszLine)++;
        }
        szBuffer[len] = '\0';
	*pnTarget = atoi(szBuffer);
        return true;

}
