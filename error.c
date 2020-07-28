/*****************************************************************************
 * error.c: Error handling and debugging functions.
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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h> /* abort() */
#include "types.h"
#include "error.h"


/* This function adapted slightly from:
 * http://tombarta.wordpress.com/2008/08/01/c-stack-traces-with-gcc/
 * Note: Not portable -- gcc specific functions used
 */
#include <execinfo.h>
void print_trace(FILE *out, const char *function) {
	const size_t max_depth = 100;
	size_t stack_depth;
	void *stack_addrs[max_depth];
	char **stack_strings;
	
	stack_depth = backtrace(stack_addrs, max_depth);
	stack_strings = backtrace_symbols(stack_addrs, stack_depth);
	fprintf(out, "Call stack from %s:\n", function);
	
	size_t i;
	for (i = 1; i < stack_depth; i++) {
		fprintf(out, "    %s\n", stack_strings[i]);
	}
	free(stack_strings); // malloc()ed by backtrace_symbols
	fflush(out);
}


/* Debugging output to stderr */
#ifndef NDEBUG
void Debug_(PCSZ pszFunction, PCSZ pszFormat, ...) {
        va_list args;

        fprintf(stderr, "DEBUG (%s): ", pszFunction);
        va_start(args, pszFormat);
        vfprintf(stderr, pszFormat, args);
        va_end(args);
        fprintf(stderr, "\n");
}


/* Called upon failure of an assertion */
void AssertFail(PCSZ pszExp, PCSZ pszFile, int nLine, PCSZ pszFunction) {
	Die_(pszFunction, "%s@%d: Assertion `%s' failed", pszFile, nLine, pszExp);
}


#endif


/* Aborts the program with a printf-formatted message upon event of an
 * unrecoverable program error */
void Die_(PCSZ pszFunction, PCSZ pszFormat, ...) {
	va_list args;
	va_start(args, pszFormat);
	print_trace(stderr, pszFunction);
	fprintf(stderr, "ERROR (%s): ", pszFunction);
	vfprintf(stderr, pszFormat, args);
	fprintf(stderr, "\n");
	abort();
}


/* Aborts the program upon a memory allocation failure */
void VerifyAlloc_(void* pvAlloc, PCSZ pszFunction, PCSZ pszWhat, ...) {
	if (pvAlloc)
		return;

	va_list args;
	va_start(args, pszWhat);
	print_trace(stderr, pszFunction);
	fprintf(stderr, "ERROR: (%s) Memory allocation failed for ", pszFunction);
	vfprintf(stderr, pszWhat, args);
	fprintf(stderr, "\n");
	abort();
}
