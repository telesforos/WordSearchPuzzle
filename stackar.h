// Stack class interface: array implementation
// Etype: must have zero-parameter constructor and operator=
// CONSTRUCTION: with (a) no initializer;
// copy construction of Stack objects is DISALLOWED
// deep copy is supported
// 
// --------- PUBLIC OPERATIONS --------------
// void Push( Etype X ) <-> Insert X
// void Pop( ) _-> Remove most recently inserted item
// Etype Top( ) --> Return most recentl
// int IsEmpty( )--> Return 1 if full; else return 0
// int IsFull( )--> Return 1 if empty; else return 0
// void MakeEmpty( )--> Remove all items
// ----------ERRORS -----------
// PREDEFINED exception is propogated if new fails
// EXCEPTION is called for Top of Pop on empty stack

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
	const Stack & operator = ( const Stack & Rhs );
	void Push( const Etype & X );
	void Pop( );
	const Etype & Top( ) const ;
	int IsEmpty( ) const { return TopOfStack == -1; }
	int IsFull( ) const { return 0; }
	void MakeEmpty( ) { TopOfStack = -1; }
private:
// Copy constructor remains disabled by inheritance
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
		delete [ ] Array;
		MaxSize = Rhs.MaxSize;
		TopOfStack = Rhs.TopOfStack;
		Array = new Etype[MaxSize];
		for ( int i = 0; i <= TopOfStack; i++ )
			Array[i] = Rhs.Array[i];
	}
	return *this;
}

template <class Etype>
void Stack<Etype>::Push( const Etype & X )
{
	if ( TopOfStack + 1 == MaxSize )
	{
		// Stack is full, need to expand
		Etype *OldArray = Array;
		MaxSize *= 2;
		Array = new Etype[MaxSize];
		for ( int i = 0; i <= TopOfStack; i++ )
			Array[i] = OldArray[i];
		delete [ ] OldArray;
	}
	Array[++TopOfStack] = X;
}

template <class Etype>
void Stack<Etype>::Pop( )
{
	if ( IsEmpty( ) )
		EXCEPTION( 1, "Pop on empty stack" );
	TopOfStack--;
}

template <class Etype>
const Etype & Stack<Etype>::Top( ) const
{
	if ( IsEmpty( ) )
		EXCEPTION( 1, "Top of empty stack" );
	return Array[TopOfStack];
}

#endif
