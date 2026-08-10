#include "rndm.h"
#include <cmath>
using namespace std;

// Moduli for the two component recurrences of L'Ecuyer's combined generator.
static const long M1 = 2147483563L;
static const long M2 = 2147483399L;

static const double PI = 3.14159265358979323846;

// Constructor
Rndm::Rndm(long Sd1, long Sd2) : Seed1(Sd1), Seed2(Sd2)
{
	Normalize();
}

// Set seeds
void Rndm::SetSeeds(long Sd1, long Sd2)
{
	Seed1 = Sd1;
	Seed2 = Sd2;
	Normalize();
}

// Map arbitrary (possibly zero or negative) seeds into the required ranges:
// Seed1 in 1..M1-1 and Seed2 in 1..M2-1. Zero is a fixed point for these
// recurrences, so it must be excluded.
void Rndm::Normalize()
{
	Seed1 %= (M1 - 1);
	if (Seed1 < 0)
		Seed1 += (M1 - 1);
	Seed1 += 1;

	Seed2 %= (M2 - 1);
	if (Seed2 < 0)
		Seed2 += (M2 - 1);
	Seed2 += 1;
}

// Uniform real number strictly between 0 and 1
double Rndm::UniformReal(void)
{
	// Each recurrence uses Schrage's method so no intermediate product
	// exceeds the range of a 32-bit long.
	long K = Seed1 / 53668L;
	Seed1 = 40014L * (Seed1 - K * 53668L) - K * 12211L;
	if (Seed1 < 0)
		Seed1 += M1;

	K = Seed2 / 52774L;
	Seed2 = 40692L * (Seed2 - K * 52774L) - K * 3791L;
	if (Seed2 < 0)
		Seed2 += M2;

	// Combine. Result lies in 1..M1-1, so the quotient is in (0,1) and can
	// never be exactly 0.0 or 1.0 -- NegExpoReal's log() depends on that.
	long Z = Seed1 - Seed2;
	if (Z < 1)
		Z += M1 - 1;

	return (double)Z / (double)M1;
}

// Uniform real number between LowerBound and UpperBound
double Rndm::UniformReal(double LowerBound, double UpperBound)
{
	return LowerBound + (UpperBound - LowerBound) * UniformReal();
}

// Uniform long integer in LowerBound..UpperBound inclusive
long Rndm::UniformLong(long LowerBound, long UpperBound)
{
	if (UpperBound <= LowerBound)
		return LowerBound;

	const long Span = UpperBound - LowerBound + 1;
	long Result = LowerBound + (long)(Span * UniformReal());

	// Belt and braces against floating point rounding at the top of the range.
	if (Result > UpperBound)
		Result = UpperBound;

	return Result;
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
	// Per-object rather than per-class: the previous version cached the spare
	// deviate in function-level statics, so two Rndm objects handed each other
	// values drawn from the wrong seed stream.
	if (HaveSpare)
	{
		HaveSpare = false;
		return Mean + StdDev * Spare;
	}

	HaveSpare = true;
	double U = UniformReal();
	double V = UniformReal();
	double S = sqrt(-2.0 * log(U));
	Spare = S * sin(2.0 * PI * V);
	return Mean + StdDev * S * cos(2.0 * PI * V);
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
