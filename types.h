/*****************************************************************************
 * types.h: Standard generally useful types.
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

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

/* On Linux, we get "uint" from standard header linux/types.h */
#ifndef _LINUX_TYPES_H
typedef unsigned int  uint;
#endif

typedef char*         PSZ;
typedef const char*   PCSZ;

