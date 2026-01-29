#ifndef STRING_H
#define STRING_H

// String class interface: support operations for strings
// CONSTRUCTION: with (a) no initializer or (b) a
// const char * (or char *), or (c) another String
//
// PUBLIC OPERATIONS 
// = --> Usual assignment
// --> Indexing with bounds check
// ==, !=, <, <=, >, >= -~-> Usual relational and equality
// << and >> -~> Input and output
// unsigned int Length( ) --> Return strlen equivalent

#include <iostream>
#include <cstring>

class String
{
	public:
	// Constructors
	String( ): Buffer( NullString ), BufferLen( -1 ) { }
	String( const char * Value );
	String( const String & Value );

	// Destructor
	~String( ) { if( BufferLen != -1 ) delete [ ] Buffer; }

	// Assignment operator
	const String & operator=( const String & Rhs );
	const String & operator=( const char * Rhs );

	// Extras for word search puzzle; sloppy implementation there
	const String & operator=( char Rhs );
	const String & operator+=( char Rhs );

	char operator[ ]( int Index ) const;
	char & operator[ ]( int Index );

	// Type cast to char *
	//operator const char * ( ) const { return Buffer; }

	// Get the length
	int Length( ) const { return (int)strlen( Buffer ); }

	// Friends for comparison
	friend int operator == ( const String & Lhs,const String & Rhs );
	friend int operator != ( const String & Lhs,const String & Rhs );
	friend int operator < ( const String & Lhs,const String & Rhs );
	friend int operator > ( const String & Lhs,const String & Rhs );
	friend int operator <= ( const String & Lhs,const String & Rhs );
	friend int operator >= ( const String & Lhs,const String & Rhs );

	friend std::ostream & operator<<( std::ostream & Out,const String & Value);

	private:
		char *Buffer;// Stores the chars
		int BufferLen;// Max strlen for Buffer
		static char *NullString; // Member for uninitialized case
		void GetBuffer( unsigned int MaxStrLen );
};
	std::istream & operator>>( std::istream & In,String & Value );
#endif
