#ifndef VECTOR_H
#define VECTOR_H

// Vector class interface: support bounds-checked arrays
// Etype: must have zero-parameter constructor and operator=
// CONSTRUCTION: with (a) an integer size only
// [ J] --> Indexing with bounds check
// int Length( ) -~-> Return # elements in Vector
// void Resize( int NewSize ) --> Change bounds

// void Double( ) --> Double Vector capacity

#include <iostream>
#include <algorithm>
#include "Exception.h"
using namespace std;

template <class Etype>
class Vector
{
public:
	// Constructors
	Vector ( int Size );

	// Destructor
	~Vector ( ) { delete [ ] Array; }

	// Index the Array
	const Etype & operator[] ( int Index ) const;
	Etype & operator [] ( int Index );

	// Copy Identically Sized Arrays
	const Vector & operator=( const Vector & Rhs );

	// Get. the Length
	int Length ( ) const { return ArraySize; }
	// Resize the Array
	void Resize ( int NewSize );
	void Double ( ) { Resize ( ArraySize * 2 ); }
protected: // Changed from textbook for WordSrch project (was private)
	Etype * Array;
	int ArraySize;

	void GetArray( ); // Call new and check for errors

	// Disable Copy constructor
	Vector ( const Vector & Rhs );
};

// Template implementations
template <class Etype>
Vector<Etype>::Vector ( int Size ) : ArraySize( Size )
{
	GetArray( );
}

template <class Etype>
const Etype &
Vector<Etype>::operator[] (int Index) const
{
    EXCEPTION( Index < 0 || Index >= ArraySize,"index out of range" );
    return Array[Index];
}

template <class Etype>
Etype &
Vector<Etype>::operator[] (int Index)
{
    EXCEPTION( Index < 0 || Index >= ArraySize,"index out of range" );
    return Array[Index];
}

template <class Etype>
const Vector<Etype> &
Vector<Etype>::operator=( const Vector<Etype> & Rhs )
{
    if( this != &Rhs )
    {
        EXCEPTION( ArraySize != Rhs.ArraySize, "Incompatible array sizes");

        for(int i = 0; i < ArraySize; i++)
            Array[ i ] = Rhs.Array[i];
    }
    return *this;
}

template <class Etype>
void
Vector<Etype>::Resize( int NewSize)
{
    Etype *OldArray = Array;
    const int MinOfOldAndNew = min( ArraySize, NewSize);

    ArraySize = NewSize;
    GetArray();
    for( int i =0; i < MinOfOldAndNew;i++)
        Array[i] = OldArray[i];
    delete [] OldArray;
}

template <class Etype>
void Vector<Etype>::GetArray( )
{
	Array = new Etype[ ArraySize ];
	if ( Array == NULL )
		EXCEPTION( 1, "Out of memory" );
}

#endif
