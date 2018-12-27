// Greatest common divisor and least common multiple
#pragma once
#include "../config.h"

namespace UTIL
{

//
// gcd is an algorithm that calculates the greatest common divisor of two
//  integers, using Euclid's algorithm.
//
// Pre: A > 0 && B > 0
// Recommended: A > B
template <typename Integer> 
inline Integer gcd(Integer A, Integer B)
{
	do
	{
		const Integer tmp(B);
		B = A % B;
		A = tmp;
	} while (B != 0);

	return A;
};

//
// lcm is an algorithm that calculates the least common multiple of two
//  integers.
//
// Pre: A > 0 && B > 0
// Recommended: A > B
template <typename Integer> 
inline Integer lcm(const Integer & A, const Integer & B)
{
	Integer ret = A;
	ret /= gcd(A, B);
	ret *= B;
	return ret;
};

#ifdef _DEBUG
inline void EnableMemLeakCheck()
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
};
#endif

};