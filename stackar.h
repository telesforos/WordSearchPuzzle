// Stack class interface: array implementation
// Etype: must have zero-parameter constructor and operator=
// CONSTRUCTION: with (a) no initializer;
// copy construction of Stack objects is DISALLOWED
// deep copy assignment is supported
//
// --------- PUBLIC OPERATIONS --------------
// void Push( Etype X ) --> Insert X; grows the array as needed
// void Pop( ) --> Remove most recently inserted item
// Etype Top( ) --> Return most recently inserted item
// int IsEmpty( ) --> Return 1 if empty; else return 0
// int IsFull( ) --> Return 0 always; this stack grows on demand
// void MakeEmpty( ) --> Remove all items
// ----------ERRORS -----------
// std::bad_alloc is propagated if new fails
// RangeError is thrown for Top or Pop on an empty stack

#ifndef STACKAR_H
#define STACKAR_H
#include "absstack.h"
#include "Exception.h"

template <class Etype>
class Stack : public AbsStack<Etype>
{
public:
	Stack( );
	~Stack( ) { delete [ ] Array; }
	Stack( const Stack & Rhs ) = delete;
	const Stack & operator = ( const Stack & Rhs );
	void Push( const Etype & X );
	void Pop( );
	const Etype & Top( ) const ;
	int IsEmpty( ) const { return TopOfStack == -1; }
	int IsFull( ) const { return 0; }
	void MakeEmpty( ) { TopOfStack = -1; }
private:
	int MaxSize;
	int TopOfStack;
	Etype *Array;
};

// Template implementations
template <class Etype>
Stack<Etype>::Stack( )
{
	MaxSize = 10;
	TopOfStack = -1;
	Array = new Etype[MaxSize];
}

template <class Etype>
const Stack<Etype> & Stack<Etype>::operator = ( const Stack & Rhs )
{
	if ( this != &Rhs )
	{
		// Build the replacement before destroying the original so a failed
		// allocation leaves this stack untouched.
		Etype *NewArray = new Etype[Rhs.MaxSize];
		for ( int i = 0; i <= Rhs.TopOfStack; i++ )
			NewArray[i] = Rhs.Array[i];

		delete [ ] Array;
		Array = NewArray;
		MaxSize = Rhs.MaxSize;
		TopOfStack = Rhs.TopOfStack;
	}
	return *this;
}

template <class Etype>
void Stack<Etype>::Push( const Etype & X )
{
	if ( TopOfStack + 1 == MaxSize )
	{
		// Stack is full, need to expand
		Etype *NewArray = new Etype[MaxSize * 2];
		for ( int i = 0; i <= TopOfStack; i++ )
			NewArray[i] = Array[i];
		delete [ ] Array;
		Array = NewArray;
		MaxSize *= 2;
	}
	Array[++TopOfStack] = X;
}

template <class Etype>
void Stack<Etype>::Pop( )
{
	EXCEPTION( IsEmpty( ), "Pop on empty stack" );
	TopOfStack--;
}

template <class Etype>
const Etype & Stack<Etype>::Top( ) const
{
	EXCEPTION( IsEmpty( ), "Top of empty stack" );
	return Array[TopOfStack];
}

#endif
