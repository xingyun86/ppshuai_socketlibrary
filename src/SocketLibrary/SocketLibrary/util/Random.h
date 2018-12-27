/*
 * File: random.h
 * Version: 1.0
 * Last modified on Fri Jul 22 16:44:36 1994 by eroberts
 * -----------------------------------------------------
 * This interface provides several functions for generating
 * pseudo-random numbers.
 */

#ifndef RANDOM_H
#define RANDOM_H
#pragma once
#include "../config.h"

#ifndef RAND_MAX
#define RAND_MAX ((int) ((unsigned) ~0 >> 1))
#endif

namespace UTIL
{

class SOCKETLIBRARY_API CRandom
{
public:

	/*
	* Function: Randomize
	* Usage: Randomize();
	* -------------------
	* This function sets the random seed so that the random sequence
	* is unpredictable.  During the debugging phase, it is best not
	* to call this function, so that program behavior is repeatable.
	*/
	static void    Randomize(int nSend = 0);

	/*
	* Function: RandomInteger
	* Usage: n = RandomInteger(low, high);
	* ------------------------------------
	* This function returns a random integer in the range low to high,
	* inclusive.
	*/
	static int	   RandomInteger(); //random number range [0,RAND_MAX]
	static int	   RandomInteger(int nLow, int nHigh);

	/*
	* Function: RandomReal
	* Usage: d = RandomReal(low, high);
	* ---------------------------------
	* This function returns a random real number in the half-open
	* interval [low .. high), meaning that the result is always
	* greater than or equal to low but strictly less than high.
	*/
	static double  RandomReal(); //random number range [0,1]
	static double  RandomReal(double dLow, double dHigh);

	/*
	* Function: RandomChance
	* Usage: if (RandomChance(p)) . . .
	* ---------------------------------
	* The RandomChance function returns TRUE with the probability
	* indicated by p, which should be a floating-point number between
	* 0 (meaning never) and 1 (meaning always).  For example, calling
	* RandomChance(.30) returns TRUE 30 percent of the time.
	*/
	static bool RandomChance(double p);

};

};
#endif
