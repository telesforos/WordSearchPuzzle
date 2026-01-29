#include "rndm.h"
#include <cmath>
using namespace std;

// Constructor
Rndm::Rndm(long Sd1, long Sd2) : Seed1(Sd1), Seed2(Sd2)
{
	if (Seed1 == 0) Seed1 = 1;
	if (Seed2 == 0) Seed2 = 1;
}

// Set seeds
void Rndm::SetSeeds(long Sd1, long Sd2)
{
	Seed1 = Sd1;
	Seed2 = Sd2;
	if (Seed1 == 0) Seed1 = 1;
	if (Seed2 == 0) Seed2 = 1;
}

// Uniform real number between 0 and 1
double Rndm::UniformReal(void)
{
	// Simple linear congruential generator
	Seed1 = (Seed1 * 16807) % 2147483647;
	return (double)Seed1 / 2147483647.0;
}

// Uniform real number between LowerBound and UpperBound
double Rndm::UniformReal(double LowerBound, double UpperBound)
{
	return LowerBound + (UpperBound - LowerBound) * UniformReal();
}

// Uniform long integer between LowerBound and UpperBound
long Rndm::UniformLong(long LowerBound, long UpperBound)
{
	return LowerBound + (long)((UpperBound - LowerBound + 1) * UniformReal());
}

// Negative exponential distribution
double Rndm::NegExpoReal(double Mean)
{
	return -Mean * log(UniformReal());
}

// Negative exponential long
long Rndm::NegExpoLong(double Mean)
{
	return (long)NegExpoReal(Mean);
}

// Normal distribution using Box-Muller transform
double Rndm::NormalReal(double Mean, double StdDev)
{
	static int HaveSpare = 0;
	static double Spare;
	
	if (HaveSpare)
	{
		HaveSpare = 0;
		return Mean + StdDev * Spare;
	}
	
	HaveSpare = 1;
	double U = UniformReal();
	double V = UniformReal();
	double S = sqrt(-2.0 * log(U));
	Spare = S * sin(2.0 * 3.14159265358979 * V);
	return Mean + StdDev * S * cos(2.0 * 3.14159265358979 * V);
}

// Normal distribution long
long Rndm::NormalLong(double Mean, double StdDev)
{
	return (long)NormalReal(Mean, StdDev);
}

// Poisson distribution
unsigned int Rndm::Poisson(double Mean)
{
	double ExpMinus = exp(-Mean);
	double Product = UniformReal();
	unsigned int N = 0;
	
	while (Product > ExpMinus)
	{
		Product *= UniformReal();
		N++;
	}
	return N;
}
