/*****************************************************************************
 * main.c: Genetic algorithm implementation of Robby the Soda-Can-Collecting 
 * Robot. Program entry point and top-level algorithm control.
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
#include <time.h>
#include <unistd.h>
#include "types.h"
#include "error.h"
#include "main.h"
#include "misc.h"
#include "strategy.h"
#include "population.h"
#include "world.h"
#include "robby.h"


#define GENERALIZATION_SESSIONS		1000.0


/* Local functions */
void CalculateFitness(ARGS const* pArgs, POPULATION* pPopulation, const WORLD* pWorld);
double CalculateGeneralization(ARGS const* pArgs, STRATEGY* pstg, const WORLD* pWorld);
void EvolveNewPopulation(const ARGS* pArgs, POPULATION* pPopOld, POPULATION* pPopNew);
void MateStrategies(const STRATEGY* pstgMother, const STRATEGY* pstgFather, STRATEGY* pstgChild, int iactCrossover);
void MutateStrategy(double rMutationProbability, STRATEGY* pstg);
void PrintWelcome(void);
void ProcessCommandLine(int argc, char** argv, ARGS* pArgs);
void SortByFitness(POPULATION* pPopulation);
void Usage();


/*
 * Default program parameters
 */
const ARGS k_argsDefault = {
	.nPopulationSize  = 200,
	.cGenerations     = 500,
	.cSessions        = 200,
	.cSessionActions  = 200,
	.rMutationProbability = 0.005,
	.rCanProbability  = 0.5,
	.nSeed            = 8675309,
	.pszWorld         = "default.world",
	.bUseCrossover    = true,
	.robbyType        = NormalRobby
};


/* Print program command-line usage */
void Usage() {
	const ARGS* p = &k_argsDefault;
	fprintf(stderr, "Usage: ./robby ARGS\n");
	fprintf(stderr, "Where ARGS is zero or more of:\n");
	fprintf(stderr, "\t-p <Population Size>      (default: %d)\n", p->nPopulationSize);
	fprintf(stderr, "\t-g <Generations>          (default: %d)\n", p->cGenerations);
	fprintf(stderr, "\t-s <Sessions>             (default: %d)\n", p->cSessions);
	fprintf(stderr, "\t-a <Actions Per Session>  (default: %d)\n", p->cSessionActions);
	fprintf(stderr, "\t-m <Mutation Probability> (default: %g)\n", p->rMutationProbability);
	fprintf(stderr, "\t-c <Can Probability>      (default: %g)\n", p->rCanProbability);
	fprintf(stderr, "\t-r <Random number seed>   (default: %d)\n", p->nSeed);
	fprintf(stderr, "\t-w <World file to use>    (default: %s)\n", p->pszWorld);
	fprintf(stderr, "\t-x: Turn off crossover\n");
	fprintf(stderr, "\t-z <id|smart> Use IDRobby (custom; no evolution/mutation) or SmartRobby\n");
	fprintf(stderr, "\t-h: Display this help message and exit\n");
}


/* Process command line, updating caller's ARGS struct */
void ProcessCommandLine(int argc, char** argv, ARGS* pArgs) {
	int ch;
	const char szArgOptions[]   = "pgsamcrwxz"; /* Options with an argument */
	const char szGetOptString[] = "p:g:s:a:m:c:r:w:z:hx"; /* All options */

	opterr = 0;
	while ((ch = getopt(argc, argv, szGetOptString)) != -1) {
		switch (ch) {
		case 'p':
			pArgs->nPopulationSize = atoi(optarg);
			printf("# Population Size: %d\n", pArgs->nPopulationSize);
			break;
		case 'g':
			pArgs->cGenerations = atoi(optarg);
			printf("# Generations:     %d\n", pArgs->cGenerations);
			break;
		case 's':
			pArgs->cSessions = atoi(optarg);
			printf("# Sessions:        %d\n", pArgs->cSessions);
			break;
		case 'a':
			pArgs->cSessionActions = atoi(optarg);
			printf("# Session Actions: %d\n", pArgs->cSessionActions);
			break;
		case 'm':
			pArgs->rMutationProbability = atof(optarg);
			printf("# Mutation Prob:   %g\n", pArgs->rMutationProbability);
			break;
		case 'c':
			pArgs->rCanProbability = atof(optarg);
			printf("# Can Probability: %g\n", pArgs->rCanProbability);
			break;
		case 'r':
			pArgs->nSeed = atoi(optarg);
			printf("# Random seed:     %d\n", pArgs->nSeed);
			break;
		case 'w':
			pArgs->pszWorld = optarg;
			printf("# World file:      %s\n", pArgs->pszWorld);
			break;
		case 'x':
			pArgs->bUseCrossover = false;
			printf("# Crossover:       off\n");
			break;
		case 'z':
			if (strcmp(optarg, "id") == 0) {
				pArgs->robbyType = IdRobby;
				printf("# Using IntelligentDesignRobby\n");
			} else if (strcmp(optarg, "smart") == 0) {
				pArgs->robbyType = SmartRobby;
				printf("# Using SmartRobby\n");
			} else
				fprintf(stderr, "Unrecognized -z parameter\n");
			break;
		case 'h':
			Usage();
			exit(EXIT_SUCCESS);
			break;
		case '?':
			if (strchr(szArgOptions, optopt) == NULL)
				fprintf(stderr, "Unknown option -%c\n", optopt);
			else
				fprintf(stderr, "Option -%c requires an argument\n", optopt);
			Usage();
			exit(EXIT_FAILURE);
			break;
		default:
			abort();
		}
	}

	int i;
	for (i = optind; i < argc; ++i)
		fprintf(stderr, "** WARNING: Argument '%s' ignored\n", argv[i]);
}


/* Calculate fitness of a population */
void CalculateFitness(ARGS const* pArgs, POPULATION* pPopulation, const WORLD* pWorld) {
	ASSERT(pArgs && pPopulation && pWorld);

	/* Create a world to play in */
	WORLD* pwldCurrent = WorldCreate(pWorld->cx, pWorld->cy);

	int istg, iSession, nScoreSum;
	for (istg = 0; istg < pPopulation->cstg; ++istg) {
		/* For each strategy, try it out "cSessions" times and take
		 * the average score */
		nScoreSum = 0;
		for (iSession = 0; iSession < pArgs->cSessions; ++iSession) {
			/* Start with a fresh world with randomly placed cans
			 * for each cleaning run */
			WorldCopy(pWorld, pwldCurrent);
			WorldSetCansRandomly(pwldCurrent, pArgs->rCanProbability);
			//WorldDump(pwldCurrent, stdout);
			nScoreSum += RobbyClean(pArgs, pwldCurrent, &pPopulation->rgstg[istg], pArgs->cSessionActions);
		}
		pPopulation->rgstg[istg].rFitness = (double)nScoreSum / (double)pArgs->cSessions;
		//Debug("Strategy %d: %g", istg, pPopulation->rgstg[istg].rFitness);
	}
	WorldDestroy(pwldCurrent);
}


/* Calculate generalization score for a strategy */
double CalculateGeneralization(ARGS const* pArgs, STRATEGY* pstg, const WORLD* pWorld) {
	ASSERT(pArgs && pstg && pWorld);

	/* Create a world to play in */
	WORLD* pwldCurrent = WorldCreate(pWorld->cx, pWorld->cy);

	int iSession, nScoreSum = 0;
	for (iSession = 0; iSession < GENERALIZATION_SESSIONS; ++iSession) {
		WorldCopy(pWorld, pwldCurrent);
		WorldSetCansRandomly(pwldCurrent, pArgs->rCanProbability);
		nScoreSum += RobbyClean(pArgs, pwldCurrent, pstg, pArgs->cSessionActions);
	}
	WorldDestroy(pwldCurrent);
	return (double)nScoreSum / GENERALIZATION_SESSIONS;
}


/* Selects a parent for mating based on fitness rank of a (pre-sorted) population */
int SelectParent(POPULATION* pPop) {
	ASSERT(pPop);

	const int nPopSize = pPop->cstg;
	int istg = rand() % nPopSize;
	int cTries = nPopSize;
	/* sum = 1 + 2 + ... + n (where n is population size) or
	 * n(n+1)/2 */
	double sum = (double) (nPopSize * (nPopSize + 1) / 2);
	while (cTries-- > 0) {
		/* istg is the index of the strategy we're looking at *and*
		 * its (0-based) rank.
		 * The probability that each individual will be chosen:
		 *    POPULATION_SIZE - fitness_rank + 1
		 *   ------------------------------------
		 *    1 + 2 + ... + POPULATION_SIZE
		 */
		double rRandom = RandomZeroOne();
		double rProb = (double)(nPopSize - istg + 1) / sum;
		if (rRandom < rProb)
			return istg;

		istg = (istg + 1) % nPopSize;
	}
	return rand() % nPopSize;
}


/* Mates two strategies given a crossover index, putting resulting ACTION set into child */
void MateStrategies(
	const STRATEGY* pstgMother,
	const STRATEGY* pstgFather,
	STRATEGY* pstgChild,
	int iactCrossover)
{
	ASSERT(pstgMother && pstgFather && pstgChild);
	ASSERT(iactCrossover >= 0 && iactCrossover < STRATEGY_LENGTH);

	size_t as = sizeof(pstgMother->rgact[0]);
	int cactMother = iactCrossover;
	int cactFather = STRATEGY_LENGTH - iactCrossover;

	memcpy(&pstgChild->rgact[0], &pstgMother->rgact[0], as * cactMother);
	memcpy(&pstgChild->rgact[iactCrossover], &pstgFather->rgact[iactCrossover], as * cactFather);
}


/* Mutate given strategy. "For each number in the child's chromosome, with
 * probability MUTATION PROBABILITY replace that number with a randomly
 * generated number between 0 and 6." */
void MutateStrategy(double rMutationProbability, STRATEGY* pstg) {
	int iact;
	for (iact = 0; iact < STRATEGY_LENGTH; ++iact) {
		if (RandomZeroOne() < rMutationProbability)
			pstg->rgact[iact] = rand() % NUM_ACTIONS;
	}
}


/* Evolves a complete, new population from an existing one using crossover/
 * cloning, and genetic mutation. */
void EvolveNewPopulation(const ARGS* pArgs, POPULATION* pPopOld, POPULATION* pPopNew) {
	ASSERT(pPopOld && pPopNew);
	ASSERT(pPopOld->maxstg == pPopNew->maxstg);

	PopulationEmpty(pPopNew);
	while (!PopulationIsFull(pPopNew)) {
		STRATEGY* pstgMother;
		STRATEGY* pstgFather;
		int istgMother, istgFather;
		STRATEGY stgSon, stgDaughter;

		/* Pick parents via "roulette-wheel" selection */
		istgMother = SelectParent(pPopOld);
		istgFather = SelectParent(pPopOld);
		ASSERT(istgMother >= 0 && istgMother < pPopOld->cstg);
		ASSERT(istgFather >= 0 && istgFather < pPopOld->cstg);

		pstgMother = &pPopOld->rgstg[istgMother];
		pstgFather = &pPopOld->rgstg[istgFather];

		if (pArgs->bUseCrossover) {
			/* Mate the parent strategies to form two children using same
			 * crossover point, but switching parent order for second child, to
			 * get both combinations of this specific crossover point */
			int iactCrossover = rand() % STRATEGY_LENGTH;
			MateStrategies(pstgMother, pstgFather, &stgSon, iactCrossover);
			MateStrategies(pstgFather, pstgMother, &stgDaughter, iactCrossover);
		} else {
			/* Don't use crossover; just clone mother and father, and mutate */
			StrategyCopy(pstgMother, &stgDaughter);
			StrategyCopy(pstgFather, &stgSon);
		}
		/* Either way, mutate the children before adding them */
		MutateStrategy(pArgs->rMutationProbability, &stgSon);
		MutateStrategy(pArgs->rMutationProbability, &stgDaughter);
		PopulationAddStrategy(pPopNew, &stgSon);
		PopulationAddStrategy(pPopNew, &stgDaughter);

	}
	ASSERT(pPopOld->cstg == pPopNew->cstg);
}


/* Robby's welcome message */
void PrintWelcome(void) {
	time_t tmNow = time(NULL);
	char* szDateTime = ctime(&tmNow);
	printf("# Robby the Soda-Can-Collecting Robot\n");
	printf("# %s", szDateTime);
}


/* Program entry point */
int main(int argc, char** argv) {
	/* start with default values */
	ARGS args = k_argsDefault;
	WORLD* pwld;
	double rGeneralization;

	PrintWelcome();

	ProcessCommandLine(argc, argv, &args);
	srand(args.nSeed);
	pwld = WorldCreateFromFile(args.pszWorld);

	if (args.robbyType == NormalRobby || args.robbyType == SmartRobby) {
		POPULATION* pPopCurrent;
		POPULATION* pPopOther;

		/* Only need two populations; the current generation's population,
		 * and one to build the next generation into. We can just swap
		 * them after every generation. */
		pPopCurrent = PopulationCreate(args.nPopulationSize);
		pPopOther   = PopulationCreate(args.nPopulationSize);
		PopulationRandomize(pPopCurrent);
	
		puts("#\n# Generation\tScore");
		int iGeneration = 0;
		for (;;) {
			CalculateFitness(&args, pPopCurrent, pwld);
			PopulationSortByFitness(pPopCurrent);
	
			printf("%d\t\t%g\n", iGeneration + 1, pPopCurrent->rgstg[0].rFitness);
			if (++iGeneration >= args.cGenerations)
				break;
			EvolveNewPopulation(&args, pPopCurrent, pPopOther);
			SwapPointers((void**)&pPopCurrent, (void**)&pPopOther);
		}
		rGeneralization = CalculateGeneralization(&args, &pPopCurrent->rgstg[0], pwld);
	
		PopulationDestroy(pPopOther);
		PopulationDestroy(pPopCurrent);
	} else {
		ASSERT(args.robbyType == IdRobby);
		STRATEGY stgId;
		int i;
		/* Program Robby with an "intelligent" strategy: Pick up a can if
		 * you're on one, move toward a can if there's one adjacent, move
		 * away from a wall if there's one adjacent, or, if none of the
		 * above, just make a random move */
		for (i = 0; i < STRATEGY_LENGTH; ++i) {
			STATE s = WorldGetStateFromIndex(i);
			if (s.current == CELL_CAN)
				stgId.rgact[i] = PickUpCan;
			else if (s.west == CELL_CAN)
				stgId.rgact[i] = MoveWest;
			else if (s.north == CELL_CAN)
				stgId.rgact[i] = MoveNorth;
			else if (s.east == CELL_CAN)
				stgId.rgact[i] = MoveEast;
			else if (s.south == CELL_CAN)
				stgId.rgact[i] = MoveSouth;
			else if (s.west == CELL_WALL)
				stgId.rgact[i] = MoveEast;
			else if (s.north == CELL_WALL)
				stgId.rgact[i] = MoveSouth;
			else if (s.east == CELL_WALL)
				stgId.rgact[i] = MoveWest;
			else if (s.south == CELL_WALL)
				stgId.rgact[i] = MoveNorth;
			else
				stgId.rgact[i] = MoveRandom;
		}
		rGeneralization = CalculateGeneralization(&args, &stgId, pwld);
	}
	printf("# Generalization score: %g\n", rGeneralization);

	WorldDestroy(pwld);
	return 0;
}
