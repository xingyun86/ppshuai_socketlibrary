/*
 * File: random.h
 * Version: 1.0
 * Last modified on Fri Jul 22 16:44:36 1994 by eroberts
 * -----------------------------------------------------
 * This interface provides several functions for generating
 * pseudo-random numbers.
 */

#include "random.h"
#include <stdlib.h>
#include <time.h>

namespace UTIL
{

/*
 * Function: Randomize
 * -------------------
 * This function operates by setting the random number
 * seed to the current time.  The srand function is
 * provided by the <stdlib.h> library and requires an
 * integer argument.  The time function is provided
 * by <time.h>.
 */
void CRandom::Randomize(int nSeed)
{
	if (nSeed == 0)
		::srand((unsigned int)::time(NULL));
	else
		::srand(nSeed);
}

/*
 * Function: RandomInteger
 * -----------------------
 * This function first obtains a random integer in
 * the range [0..RAND_MAX] by applying four steps:
 * (1) Generate a real number between 0 and 1.
 * (2) Scale it to the appropriate range size.
 * (3) Truncate the value to an integer.
 * (4) Translate it to the appropriate starting point.
 */
int CRandom::RandomInteger()
{
	return ::rand();
};
int CRandom::RandomInteger(int nLow, int nHigh)
{
	if (nHigh < nLow)
	{
		int nTmp = nHigh;
		nHigh = nLow;
		nLow  = nTmp;
	};

    int k;
    double d;
	d = (double) (::rand() / ((double)RAND_MAX + 1));
    k = (int) (d * (nHigh - nLow + 1));
    return (nLow + k);
};

/*
 * Function: RandomReal
 * --------------------
 * The implementation of RandomReal is similar to that
 * of RandomInteger, without the truncation step.
 */
double CRandom::RandomReal()
{
	return (double) (::rand() / ((double)RAND_MAX + 1));
};
double CRandom::RandomReal(double dLow, double dHigh)
{
	if (dHigh < dLow)
	{
		double dTmp = dHigh;
		dHigh = dLow;
		dLow  = dTmp;
	};

    double d;
	d = (double) (::rand() / ((double)RAND_MAX + 1));
    return (dLow + d * (dHigh - dLow));
};

/*
 * Function: RandomChance
 * ----------------------
 * This function uses RandomReal to generate a number
 * between 0 and 100, which it then compares to p.
 */
bool CRandom::RandomChance(double p)
{
    return (RandomReal(0, 1) < p);
};

};
