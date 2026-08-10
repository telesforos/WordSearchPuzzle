#ifndef VECTOR_H
#define VECTOR_H

// Vector class interface: support bounds-checked arrays
// Etype: must have zero-parameter constructor and operator=
// CONSTRUCTION: with (a) an integer size only
// [ ] --> Indexing with bounds check
// int Length( ) --> Return # elements in Vector
// void Resize( int NewSize ) --> Change bounds
// void Double( ) --> Double Vector capacity
//
// Copy construction is disabled. Assignment requires equal sizes.
// The destructor is virtual because WordList derives from this class
// publicly; without it, deleting a WordList through a Vector<String> *
// would be undefined behaviour.

#include <iostream>
#include <algorithm>
#include "Exception.h"

template <class Etype>
class Vector
{
public:
	// Constructors
	Vector ( int Size );

	// Copy construction is not supported
	Vector ( const Vector & Rhs ) = delete;

	// Destructor
	virtual ~Vector ( ) { delete [ ] Array; }

	// Index the Array
	const Etype & operator[] ( int Index ) const;
	Etype & operator [] ( int Index );

	// Copy Identically Sized Arrays
	const Vector & operator=( const Vector & Rhs );

	// Get the Length
	int Length ( ) const { return ArraySize; }
	// Resize the Array
	void Resize ( int NewSize );
	void Double ( ) { Resize ( ArraySize * 2 ); }
protected: // Changed from textbook for WordSrch project (was private)
	Etype * Array;
	int ArraySize;

	void GetArray( ); // Call new; new itself throws on failure
};

// Template implementations
template <class Etype>
Vector<Etype>::Vector ( int Size ) : ArraySize( Size )
{
	EXCEPTION( Size < 0, "Vector size cannot be negative" );
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
    EXCEPTION( NewSize < 0, "Vector size cannot be negative" );

    Etype *OldArray = Array;
    const int MinOfOldAndNew = std::min( ArraySize, NewSize);
    const int OldSize = ArraySize;

    ArraySize = NewSize;
    try
    {
        GetArray();
    }
    catch( ... )
    {
        // Leave the object in its original, valid state before propagating.
        Array = OldArray;
        ArraySize = OldSize;
        throw;
    }

    for( int i =0; i < MinOfOldAndNew;i++)
        Array[i] = OldArray[i];
    delete [] OldArray;
}

template <class Etype>
void Vector<Etype>::GetArray( )
{
	// operator new throws std::bad_alloc on failure; it never returns NULL,
	// so the old "if ( Array == NULL )" check was unreachable.
	Array = new Etype[ ArraySize ];
}

#endif
