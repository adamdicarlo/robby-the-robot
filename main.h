/*****************************************************************************
 * main.h: Header for main.c.
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
 * This program is distributed in the hope that it will be useful,
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
	NormalRobby,
	SmartRobby,
	IdRobby,
} RobbyType;

typedef struct {
	/*  Variable                 Command line arg */
	int nPopulationSize;         /* -p */
	int cGenerations;            /* -g */
	int cSessions;               /* -s */
	int cSessionActions;         /* -a */
	double rMutationProbability; /* -m */
	double rCanProbability;      /* -c */
	int nSeed;                   /* -r */
	PCSZ pszWorld;               /* -w */
	bool bUseCrossover;          /* -x to turn off crossover*/
	RobbyType robbyType;         /* -z smart: SmartRobby, -z id: IntelligentDesignRobby */
} ARGS; /* args */
