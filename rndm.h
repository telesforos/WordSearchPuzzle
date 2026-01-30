// File Name: Rndm.h

// Project Name: WordSrch
// This class provides functions for generating uniform, exponential
// and normal pseudo-random distributions. In order for this to work properly,
// type long should be at least 32 bits and type double should have a minimum
// of a 23 bit mantissa.
//
// class Rndm
// This is a class to provides a 32 bit pseudo-random number generator.
// PUBLIC OPERATIONS
// CONSTRUCTOR:
// Rndm(long Sd1 = clock(), long Sd2 = time (NULL));
// Preconditions: none.

// Postconditions: The random number generator is constructed setting the

// initial seeds from Sd1 and Sd2. Seeds will be set from system clock

// as default.

//

// MEMBER FUNCTIONS:

// void SetSeeds(long Sd1 = clock(), long Sd2 = time(NULL));

// Preconditions: none.

// Postconditions: Resets the internal seeds to Sd1 and Sd2. If not

// supplied, seeds will be set from system clock.

//

// double UniformReal (void) ;

// Preconditions: none.

// Postconditions: Returns a uniformly distributed pseudorandom real

// in the range of 0 < x < 1.

//
// double UniformReal (double LowerBound, double UpperBound) ;

// Preconditions: LowerBound <= UpperBound.

// Postconditions: Returns a uniformly distributed pseudorandom real

// in the range of LowerBound < x < UpperBound

//

// long UniformLong(long LowerBound, long UpperBound) ;

// Preconditions: LowerBound <= UpperBound.

// Postconditions: Returns a uniformly distributed pseudorandom long

// in the range of LowerBound <= x <= UpperBound.

//

// double NegExpoReal (double Mean) ;

// Preconditions: Mean > 0. Standard deviation is approximately equal

// to the Mean.

// Postconditions: Returns a negative exponentially distributed
// pseudorandom real in the range of 0 < x < +infinity.

//

// long NegExpoLong (double Mean) ;
// Preconditions: Mean > 0. Standard deviation is approximately equal

// to the Mean.

// Postconditions: Returns a negative exponentially distributed
// pseudorandom long in the range of 0 <= x < +infinity.

//

// double NormalReal (double Mean, double StdDev) ;

// Preconditions: StdDev > 0.

// Postconditions: Returns a normally distributed pseudorandom real

// number in the range of -infinity < x < +infinity.

//

// long NormalLong(double Mean, double StdDev) ;

// Preconditions: StdDev > 0.

// Postconditions: Returns a normally distributed pseudorandom 1ong

// in the range of -infinity < x < +infinity.

//

// unsigned int Poisson(double Mean) ;

// Preconditions: Mean > 0.

// Postconditions: This function produces an unsigned pseudorandom Poisson

// distribution in the range of 0 <= x.


#ifndef RNDM_H
#define RNDM_H

#include <ctime>
#include <stddef.h>

class Rndm
{
public:
	// constructor
	Rndm(long Sdl = clock(), long Sd2 = time(NULL));

	void SetSeeds (long Sd1 = clock(), long Sd2 = time(NULL)) ;

	// Uniform Distribution

	double UniformReal(void) ;

	double UniformReal (double LowerBound, double UpperBound) ;
	long UniformLong(long LowerBound, long UpperBound) ;

	// Negative Exponential Distribution
	double NegExpoReal (double Mean) ;
	long NegExpoLong (double Mean) ;

	// Normal Distribution
	double NormalReal (double Mean, double StdDev) ;
	long NormalLong (double Mean, double StdDev) ;

	// Poisson Distribution
	unsigned int Poisson(double Mean) ;

private:
	long Seed1, Seed2;
};
#endif
