#ifndef WORDLIST_H
#define WORDLIST_H

#include <iostream>
#include "vector.h"
#include "string.h"

class WordList: public Vector<String>
{
public:
	WordList (int Size, const char Name[], int TheWidth) :
	Vector<String> (Size), Title(Name), Width(TheWidth) {}

	void SortByAlpha() ;
	void SortBySize();
	friend std::ostream & operator<< (std::ostream & Out, const WordList & Rhs) ;
private:
	String Title; // Title for entire list of words
	int Width; // Width of area available for printing
	// Disable copy constructor
	WordList (const WordList & Rhs);
};

#endif
