// File Name: Puzzle.h
// Project Name: WordSrch

// Class Puzzle
//
// This class is used to design "Word Search" puzzles.
//
// ---------------- PUBLIC OPERATIONS ----------------
// CONSTRUCTOR:
// Puzzle(const Vector<String> & TheWords, int TheHeight, int TheWidth)
//   Precondition: TheWords is a valid reference to a constructed Vector of
//   String objects that outlives this Puzzle, TheHeight > 0, TheWidth > 0.
//   Postcondition: The puzzle is ready to begin word insertion. An empty
//   attempt stack has been allocated, a two-dimensional grid has been
//   allocated of size (Height+2) by (Width+2), and the grid has been
//   initialized to an empty state.
//
// DESTRUCTOR:
// ~Puzzle()
//   Precondition: none.
//   Postcondition: the dynamically allocated attempt stack and puzzle grid
//   have been completely released.
//
// Copying is disabled: a Puzzle owns raw allocations, and the compiler's
// copy would duplicate the pointers and then free them twice.
//
// MEMBER FUNCTIONS:
// bool Design()
//   Precondition: none. Design() resets the grid itself, so it may be called
//   more than once on the same object.
//   Postcondition: Returns true if every word in the associated word list has
//   been inserted in the puzzle grid, in which case unused grid locations are
//   filled with randomly generated letters. Returns false if all the words
//   could not be placed. Use SearchExhausted() to distinguish "proved
//   impossible" from "hit the node limit".
//
// bool SearchExhausted() const
//   Postcondition: After a Design() that returned false, returns true if the
//   entire search space was explored (so no arrangement exists) and false if
//   the search was cut short by the node limit.
//
// long long NodesVisited() const
//   Postcondition: Returns the number of placement attempts made by the most
//   recent Design() call. Useful for reporting and for tests.
//
// void SetNodeLimit(long long Limit)
//   Precondition: none.
//   Postcondition: Design() will give up after Limit placement attempts.
//   A Limit of 0 or less means no limit. Defaults to DEFAULT_NODE_LIMIT.
//
// int CountOccurrences(const String & Word) const
//   Postcondition: Returns how many times Word appears in the finished grid,
//   counting all eight directions. Used to confirm each word has exactly one
//   solution.
//
// static void SeedRandom(long Seed)
//   Postcondition: Reseeds the generator shared by all Puzzle objects, so a
//   run can be reproduced exactly.
//
// FRIENDS:
// std::ostream & operator<< (std::ostream & Out, const Puzzle & Rhs)
//   Precondition: Out is a constructed and open ostream, Rhs is a constructed
//   Puzzle.
//   Postcondition: The entire contents of the Puzzle grid have been printed
//   to Out from (1..Height) by (1..Width). Rows are single spaced, columns
//   of one letter each are separated horizontally by a single space. The
//   entire puzzle grid is left aligned (no preceding spaces).
//
// ---------------- PRIVATE OPERATIONS ----------------
// MEMBER FUNCTIONS:
// void Reset()
//   Precondition: none.
//   Postcondition: The grid has been completely set Empty except for an
//   outermost boundary of WALL, and the attempt stack has been emptied.
//
// bool InsertOneWord(const Attempt & OneWord)
//   Precondition: OneWord references a valid Attempt record with
//   0 <= OneWord.ID() < Words.Length().
//   Postcondition: Returns true if able to insert the word indexed by
//   OneWord.ID() into the puzzle grid in the direction and starting position
//   indicated by the attempt record. Else returns false and the grid is
//   left unchanged. If the word is inserted, the copy count of each letter
//   position in the grid occupied by the word is incremented, and the letter
//   value set.
//
// void RemoveOneWord(const Attempt & OneWord)
//   Precondition: The word represented in the attempt record is currently
//   inserted in the puzzle grid.
//   Postcondition: The word represented by the attempt record has been removed
//   from the puzzle grid. The copy count of each letter position occupied by
//   the word has been decremented, and where that count reached zero the
//   position has been set back to EMPTY.
//
// void FillRemainder()
//   Precondition: none.
//   Postcondition: Every position which was still EMPTY in the puzzle grid
//   has been filled with a capital letter generated at random from the
//   range A..Z, and (where achievable) the camouflage letters do not
//   accidentally spell any word from the list a second time.

#ifndef PUZZLE_H
#define PUZZLE_H

#include <iostream>
#include "vector.h"
#include "String.h"
#include "stackar.h"
#include "attempt.h"

class Puzzle
{
public:
	// Placement attempts allowed before Design() gives up. The search is a
	// complete backtracking search, but its worst case is exponential, so an
	// over-subscribed grid would otherwise run effectively forever.
	//
	// Measured throughput is roughly 30 million attempts per second for
	// 15-letter words on a 15x15 grid, so this budget is about six seconds of
	// searching. That is long enough that no puzzle a person would actually
	// pose gets rejected for being merely difficult, and short enough that a
	// hopeless one still answers while the user is waiting.
	static const long long DEFAULT_NODE_LIMIT = 200000000LL;

	Puzzle (const Vector<String> & TheWords, int TheHeight =20,
			int TheWidth = 32);
	~Puzzle ();

	// A Puzzle owns raw memory; let the compiler forbid copies rather than
	// silently produce a double free.
	Puzzle (const Puzzle & Rhs) = delete;
	Puzzle & operator= (const Puzzle & Rhs) = delete;

	bool Design ();

	bool SearchExhausted () const { return Exhausted; }
	long long NodesVisited () const { return Nodes; }
	void SetNodeLimit (long long Limit) { NodeLimit = Limit; }

	int GetHeight () const { return Height; }
	int GetWidth () const { return Width; }

	int CountOccurrences (const String & Word) const;

	static void SeedRandom (long Seed) { RandGen.SetSeeds (Seed, Seed + 1); }

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

	long long NodeLimit;  // 0 or less means unlimited
	long long Nodes;      // Placement attempts made by the last Design()
	bool Exhausted;       // Did the last failed Design() cover the whole space?

	struct Offsets // Direction offsets
	{
		int Vert; // -1..1
		int Horz; // -1..1
	};
	static const Offsets Directions [NUM_DIRECTIONS ]; // Eight directions
	static Rndm RandGen;

	void Reset ();

	bool InsertOneWord (const Attempt & OneWord);
	void RemoveOneWord (const Attempt & OneWord);
	void FillRemainder ();

	bool AllWordsUnique () const;
};
#endif
