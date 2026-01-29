// Programmer: Lee Greenbank
// Course: CS 14
// File Name: Puzzle.h
// Project Name: WordSrch

// Class Puzzle

//This class is used to design "Word Search" puzzles.
//  PUBLIC OPERATIONS 
// CONSTRUCTOR:
// Puzzle(const Vector<String> & TheWords, int TheHeight, int TheWidth)

//Precondition: TheWords is a valid reference to a constructed Vector of
//String objects, TheHeight > 0, TheWidth > 0.

//  Postcondition: The puzzle is ready to begin word insertion. An empty
// attempt stack has been allocated, a two-dimensional grid has been
// allocated of size (Height+2) by (Width+2), and the grid has been
// initialized to an empty state.
// DESTRUCTOR:
// ~Puzzle()

//Pracondition: none.
// Postcondition: the dynamically allocated attempt stack and puzzle grid
// have been completely released.
// MEMBER FUNCTIONS:
// int Design ()
// Precondition: The puzzle has been newly constructed and is empty.
// Postcondition: Returns 1 if every word in the associated word list has
//been inserted in the puzzle grid. Unused grid locations are filled
// randomly generated letters. Returns 0 if an exhaustive search was
// unable to place all the words into the puzzle grid.

// FRIENDS:

//f_ostream & operator<< (ostream & Out, const Puzzle & Rhs)

// Precondition: Out is a constructed and open ostream, Rhs is a constr
//ted
// Puzzle.
// Postcondition: The entire contents of the Puzzle qrid have been printed
//
//
// to Out from (1..Height) by (1..Width). Rows are single spaced, col
// of one letter each are separated horizontally by a single space. If
// entire puzzle grid is left aligned (no preceeding spaces).

// PRIVATE OPERATIONS 

// MEMBER FUNCTIONS:
// woid Reset ()

// Precondition: none.
// ~~ Postcondition: The grid has been completely set Empty except for an
// outermost boundry of Wall.
// int InsertOneWord(const Attempt & OneWord)
// Precondition: OneWord references a valid Attempt record with
// 0 <= OneWord.ID() < Words. Length (}".
//  Postcondition: Returns 1 if able to insert the word indexed by
// OneWord.FD() into the puzzle grid in the direction and starting posiion
// indicated by the attempt record. Else it returns 0 and the grid is
// left unchanged. If the word is inserted, the copy count of each let 
// position in the grid occupied by the word is incremented, and the detter value set.

// void RemoveOneWord (const Attempt & OneWord)

// Precondition: The word represented in the attempt record is currently
// inserted in the puzzle grid.
// Postcondition: The word represented by the attempt record has been removed
// from the puzzle grid, its presence erased. The copy count of each letter
// position in the grid occupied by the word has been decremented, and
// if that updated count is zero, the letter at that Position has been attemptedf.

// void FillRemainder ()
// Precondition: none.

// Postcondition: Every position which was still Empty in the puzzle grid
// has been filled with a capital letter generated at random from the
// range A...Z

#ifndef PUZZLE_H
#define PUZZLE_H

#include <iostream>
#include "vector.h"
#include "string.h"
#include "stackar.h"
#include "attempt.h"

class Puzzle
{
public:
	/// @brief 
	/// @param TheWords 
	/// @param TheHeight 
	/// @param TheWidth 
	Puzzle (const Vector<String> & TheWords, int TheHeight =20,
			int TheWidth = 32);
	~Puzzle ();
	int Design ();

	friend std::ostream & operator<< (std::ostream & Out, const Puzzle & Rhs);
private:
	const Vector<String> & Words; // List of words to put in puzzle
	Stack<Attempt> *AttemptStack; // Stack of previously inserted words
	enum {WALL = '#', EMPTY = ' '};
	struct LetterBox
	{
		char Letter; // A..Z; WALL; EMPTY
		int Copies; // Number of words that share the Letter
	};
	int Height; // Height of puzzle
	int Width; // Width of puzzle
	LetterBox **Grid; // Two dimensional array [Height + 2] [Width +2]

	struct Offsets // Direction offsets
	{
		int Vert; // -1..1
		int Horz; // -1..1
	};
	static const Offsets Directions [NUM_DIRECTIONS ]; // Eight directions
	static Rndm RandGen;

	void Reset ();

	int InsertOneWord (const Attempt & OneWord);
	void RemoveOneWord (const Attempt & OneWord);
	void FillRemainder ();
};
#endif
