#ifndef WORDLIST_H
#define WORDLIST_H

// Class WordList
//
// A Vector<String> that also knows its puzzle title and the width available
// for printing, and can sort and display itself.
//
// ---------------- PUBLIC OPERATIONS ----------------
// WordList(int Size, const char Name[], int TheWidth)
//   Precondition: Size >= 0, Name is a null terminated string, TheWidth > 0.
//   Postcondition: A word list of the given capacity has been created.
//
// void SortByAlpha()
//   Postcondition: The words are in ascending alphabetical order.
//
// void SortBySize()
//   Postcondition: The words are ordered longest first. Placing long words
//   before short ones is what makes the puzzle search tractable: the hardest
//   constraints are satisfied while the grid is still mostly empty.
//
// bool HasDuplicate(int & First, int & Second) const
//   Postcondition: Returns true and sets First and Second to the indexes of a
//   matching pair if any word appears twice. Requires a sorted list to be
//   reliable, so call after SortByAlpha().
//
// operator<<
//   Postcondition: Writes the title, a blank line, then the words in numbered
//   columns, filled down each column in turn, within the print width supplied
//   at construction.

#include <iostream>
#include "vector.h"
#include "String.h"

class WordList: public Vector<String>
{
public:
	WordList (int Size, const char Name[], int TheWidth) :
	Vector<String> (Size), Title(Name), Width(TheWidth) {}

	// Copy construction is not supported (the base class forbids it too).
	WordList (const WordList & Rhs) = delete;
	WordList & operator= (const WordList & Rhs) = delete;

	void SortByAlpha() ;
	void SortBySize();

	bool HasDuplicate(int & First, int & Second) const;

	// Length of the longest word in the list; 0 for an empty list.
	int LongestWord() const;

	friend std::ostream & operator<< (std::ostream & Out, const WordList & Rhs) ;
private:
	String Title; // Title for entire list of words
	int Width; // Width of area available for printing
};

#endif
