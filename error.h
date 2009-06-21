/*****************************************************************************
 * error.h: Header for error.c.
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
#include "types.h"

#ifdef NDEBUG
#  define Debug(a, ...)
#  define ASSERT(exp)
#else

# define ASSERT(exp) \
	if (exp) {} else AssertFail(#exp, __FILE__, __LINE__, __FUNCTION__)

# define Debug(a, ...) Debug_(__FUNCTION__, (a), ##__VA_ARGS__)

void AssertFail(PCSZ pszExp, PCSZ pszFile, int nLine, PCSZ pszFunction);
void Debug_(PCSZ pszFunction, PCSZ pszFormat, ...);

#endif

/* ## has a special meaning when used between a comma and __VA_ARGS__ macro:
 * It deletes the comma if there are no extra (variable) arguments to avoid
 * a syntax error like VerifyAlloc_(mem, __FUNCTION__, what,). */
#define VerifyAlloc(mem, what, ...) \
	VerifyAlloc_((mem), __FUNCTION__, (what), ##__VA_ARGS__)

#define Die(a, ...) Die_(__FUNCTION__, (a), ##__VA_ARGS__)

void Die_(PCSZ pszFunction, PCSZ pszMessage, ...);
void VerifyAlloc_(void* pvAlloc, PCSZ pszFunction, PCSZ pszWhat, ...);
