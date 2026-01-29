// Programmer: Lee Greenbank
// File Name: Attempt.h :
// project Name: Wordsrch
// Class Attempt
// Used to record one attempt to place a word within a two-dimensional puzzle
// grid. Keeps track of current attempt, plus records all previous attempts.
// Default copy constructor and operator= with shallow copy are allowed and
// appropriate.

// ------------- PUBLIC OPERATIONS ---------
// CONSTRUCTORS

// Attempt()
// Precondition: none
// postcondition: default constructor, does nothing. Must be followed by
// initialization with operator=() assignment.
// Attempt (int Thewordtndex, int TheHeight, int Thewidth)
// Precondition: TheWordIndex >= 0; TheHeight > 0; TheWidth > 0.
// Postcondition: The construction parameters have been stored, the munber of
//f positions possible for an attempt has been computed and stored, and a
// first position and direction has been randomly generated.
// MEMBER FUNCTIONS:

// int ID() const
// Posteondition: returns the index of the word associated with this attempt.
// int Firstvert() const

// postcondition: returns the vertical start position for a word insertion

//attempt. Range 1..Height.

// int FirstHorz() const

// Postcondition: returns the horizontal start position for a word insertion
// attempt. Range 1..Width.

// aint Direction() const page i

// Postcondition: returns the direction index for a word insertion attempt.

// Range 0..(NUM_DIRECTIONS-1)

// void NextWord() .

//  Postcondition: Adjusts the attempt object to represent the next word in
// word index and picks a starting attempt direction and location. Resets
// direction and position attempt counts.

// int Nextrry()
// Precondition: none.
// postcondition: the appropriate HorzStart, VertStart, and/or Direct ionIndex
// have been updated to select a previously untried position or direction
// for the next attempt. Positionattempt and DirectionAttempt have been
// updated. Returns 1 if a new attempt is possible, else returns 0.

// PRIVATE OPERATIONS 

// Precondition: The Width and Height have been previously set.

// Postcondition: A random first direction index has been generated in the

// range 0.. (NUM _DIRECTIONS-1), Direction and Position attempt counts have
// been set to 1, a random horizontal starting position has been generated
// in the range 1..Width, and a random vertical starting position has been
// generated in the range 1..Height.

#ifndef ATTEMPT_H
#define ATTEMPT_H

#include "rndm.h"

const int NUM_DIRECTIONS = 8; // Possible direction indexes 0..(NUM_DIRECTIONS~1)

class Attempt
{
public:
	Attempt() {}; // Default, does no initialization, but required.
	Attempt(int TheWordIndex, int TheHeight, int TheWidth);
	int ID() const {return WordIndex;}
	int FirstVert() const {return VertStart;}
	int FirstHorz() const {return HorzStart;}
	int Direction() const {return DirectionIndex;}
	void NextWord() {WordIndex++; PickFirstTry();}
	int NextTry();
private:
	int WordIndex;			// Index of word represented by this object
	int DirectionIndex;		// 0..(NUM_DIRECTIONS - 1); current index
	int DirectionAttempt;	// 1..NUM_DIRECTIONS; number of directions tried
	int Height;				// Height of puzzle
	int Width;				// Width of puzzle
	int VertStart;			// 1..Height
	int HorzStart;			// 1..Width
	int NumPositions;		// Width * Height
	int PositionAttempt;	// 1..NumPositions; number of positions tried
	static Rndm RandGen;	// A pseudo-random number generator shared by class

	void PickFirstTry();
};
#endif
