#include "Exception.h"

void
EXCEPTION( int Condition, const char *ErrMsg )
{
	if( Condition )
		throw RangeError( ErrMsg );
}
