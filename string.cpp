#include "string.h"
#include <cctype>
using namespace std;

// Static member initialization
char *String::NullString = (char *)"";

// Constructor from const char*
String::String(const char *Value)
{
	if (Value == NULL)
	{
		Buffer = NullString;
		BufferLen = -1;
	}
	else
	{
		GetBuffer(strlen(Value));
		strcpy(Buffer, Value);
	}
}

// Copy constructor
String::String(const String &Value)
{
	if (Value.BufferLen == -1)
	{
		Buffer = NullString;
		BufferLen = -1;
	}
	else
	{
		GetBuffer(strlen(Value.Buffer));
		strcpy(Buffer, Value.Buffer);
	}
}

// Assignment from String
const String &String::operator=(const String &Rhs)
{
	if (this != &Rhs)
	{
		if (BufferLen != -1)
			delete [] Buffer;
		
		if (Rhs.BufferLen == -1)
		{
			Buffer = NullString;
			BufferLen = -1;
		}
		else
		{
			GetBuffer(strlen(Rhs.Buffer));
			strcpy(Buffer, Rhs.Buffer);
		}
	}
	return *this;
}

// Assignment from const char*
const String &String::operator=(const char *Rhs)
{
	if (Rhs == NULL)
	{
		if (BufferLen != -1)
			delete [] Buffer;
		Buffer = NullString;
		BufferLen = -1;
	}
	else
	{
		if (BufferLen != -1)
			delete [] Buffer;
		GetBuffer(strlen(Rhs));
		strcpy(Buffer, Rhs);
	}
	return *this;
}

// Assignment from char
const String &String::operator=(char Rhs)
{
	if (BufferLen != -1)
		delete [] Buffer;
	GetBuffer(2);
	Buffer[0] = Rhs;
	Buffer[1] = '\0';
	return *this;
}

// Concatenate char
const String &String::operator+=(char Rhs)
{
	int Len = Length();
	char *OldBuffer = Buffer;
	GetBuffer(Len + 1);
	strcpy(Buffer, OldBuffer);
	Buffer[Len] = Rhs;
	Buffer[Len + 1] = '\0';
	if (OldBuffer != NullString)
		delete [] OldBuffer;
	return *this;
}

// Index operator (const)
char String::operator[](int Index) const
{
	if (Index < 0 || Index >= Length())
		return '\0';
	return Buffer[Index];
}

// Index operator (non-const)
char &String::operator[](int Index)
{
	static char DummyChar = '\0';
	if (Index < 0 || Index >= Length())
		return DummyChar;
	return Buffer[Index];
}

// Get buffer - allocates memory for string
void String::GetBuffer(unsigned int MaxStrLen)
{
	Buffer = new char[MaxStrLen + 1];
	BufferLen = MaxStrLen;
}

// Comparison operators
int operator==(const String &Lhs, const String &Rhs)
{
	return strcmp(Lhs.Buffer, Rhs.Buffer) == 0;
}

int operator!=(const String &Lhs, const String &Rhs)
{
	return strcmp(Lhs.Buffer, Rhs.Buffer) != 0;
}

int operator<(const String &Lhs, const String &Rhs)
{
	return strcmp(Lhs.Buffer, Rhs.Buffer) < 0;
}

int operator>(const String &Lhs, const String &Rhs)
{
	return strcmp(Lhs.Buffer, Rhs.Buffer) > 0;
}

int operator<=(const String &Lhs, const String &Rhs)
{
	return strcmp(Lhs.Buffer, Rhs.Buffer) <= 0;
}

int operator>=(const String &Lhs, const String &Rhs)
{
	return strcmp(Lhs.Buffer, Rhs.Buffer) >= 0;
}

// Output operator
std::ostream &operator<<(std::ostream &Out, const String &Value)
{
	Out << Value.Buffer;
	return Out;
}

// Input operator
std::istream &operator>>(std::istream &In, String &Value)
{
	char Buffer[256];
	In >> Buffer;
	Value = Buffer;
	return In;
}
