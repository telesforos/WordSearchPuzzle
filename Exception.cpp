#include "Exception.h"
#include <cstdlib>
#include <iostream>
using namespace std;

void
EXCEPTION( int Condition, const char *ErrMsg )
{
	if( Condition )
	{
		cerr << "Exception: " << ErrMsg << endl;
		abort ( );
	}
}
