//Programmer: James Moore 
//Course: CS14 
//File name:puzzle.cpp 

//Project Name: Wordsrch
#include "puzzle.h"
using namespace std;

typedef int Boolean;
const Boolean TRUE = 1;
const Boolean FALSE = 0;

const Puzzle::Offsets Puzzle::Directions [NUM_DIRECTIONS] =
//Directions 0 1 2 3 4 5 6 7 

		{{0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}}; 
		
Rndm Puzzle::RandGen;

Puzzle::Puzzle(const Vector<String>& TheWords,int TheHeight,
int TheWidth):Words(TheWords)
{
	Height = TheHeight;
	Width = TheWidth;

	AttemptStack = new Stack<Attempt>;

	Grid = new LetterBox * [Height + 2];
		for (int i = 0;i < Height+2;i++)
			Grid[i] = new LetterBox [Width + 2];
		Reset();
}

Puzzle::~Puzzle()
{
	delete AttemptStack;
	for ( int i = 0; i < Height + 2 ; i++ )
		delete []Grid[i];
	delete []Grid;
}

std::ostream &
operator<< (std::ostream & Out, const Puzzle & Rhs)
{
for (int i = 1 ; i < Rhs.Height+1; i++)
	{
		for(int j = 1;j<Rhs.Width+1; j++)
		{ 
			Out <<Rhs.Grid[i] [j].Letter << ' ';
		}
		Out << endl;
	}
return Out;
}

void
Puzzle::Reset ()
{
	for(int i = 0;i<Height+2;i++)
	{
		for(int j = 0; j < Width+2;j++)
		{
			if(i == 0 || i == Height +1 || (j ==  0) || j == Width+ 1)
				Grid[i] [j].Letter = WALL;
			else
			{
				Grid[i] [j] .Letter = EMPTY;
			}
			Grid[i] [j] .Copies = 0;
		} 
	}
}

int
Puzzle::InsertOneWord(const Attempt & OneWord)
{
	int Row = OneWord.FirstVert();
	int Column = OneWord.FirstHorz();
	int Direction = OneWord.Direction();
	int WordIndex = OneWord.ID();

	for (int Ltr = 0;Ltr < Words[WordIndex].Length(); Ltr++)
	{
		if(Grid [Row] [Column].Letter == EMPTY || Grid[Row] [Column].Letter==
			Words [WordIndex] [Ltr] )
			{
				Row = Row + Directions [Direction].Vert;
				Column = Column + Directions [Direction].Horz;
			}
			else
				return 0;//failure to place word
	}

	Row = OneWord.FirstVert();
	Column = OneWord.FirstHorz();

	for(int Ltr=0;Ltr < Words [WordIndex].Length() ;Ltr++)
	{
		Grid [Row] [Column].Letter = Words [WordIndex] [Ltr];
		Grid [Row] [Column].Copies++;
		Row = Row + Directions [Direction].Vert;
		Column = Column + Directions [Direction].Horz;
	}
	return 1; //success
}
void
Puzzle::RemoveOneWord (const Attempt & OneWord)
{
	int Row = OneWord.FirstVert ();
	int Column = OneWord.FirstHorz();
	int Direction = OneWord.Direction();
	int WordIndex = OneWord.ID();
	for(int Ltr =0;Ltr < Words [WordIndex].Length() ; Ltr++)
	{
		if (Grid [Row] [Column].Copies > 1)
			Grid [Row] [Column].Copies--;
		else
		{
			Grid [Row] [Column].Copies = 0;
			Grid [Row] [Column].Letter = EMPTY;
		}
		Row = Row + Directions [Direction].Vert;
		Column = Column + Directions [Direction].Horz;
	}
}

void
Puzzle::FillRemainder()
{
	for(int i = 1;i < Height+1;i++)
	{
		for(int j = 1;j < Width+1;j++)
		{
			if( Grid[i] [j].Letter == EMPTY)
				Grid[i] [j].Letter =char( RandGen.UniformLong('A','Z'));
		}
	}
}

int
Puzzle::Design()
{
	int Tries;
	int Wordndx=0;
	Attempt LocalAttempt (Wordndx,Height, Width) ;
	
	while(LocalAttempt.ID() < Words.Length())
	{
		if (InsertOneWord (LocalAttempt) )
		{
			AttemptStack->Push (LocalAttempt) ;
			LocalAttempt .NextWord () ;
		}
		else
		{
			if (LocalAttempt .NextTry())
				Tries = TRUE;
			else
				Tries = FALSE;
		}
		while (Tries == FALSE && AttemptStack->IsEmpty()==0)
		{
			LocalAttempt=AttemptStack->Top () ;
			AttemptStack->Pop() ;
			RemoveOneWord (LocalAttempt) ;
			Tries = LocalAttempt.NextTry();
		}
		if (Tries==FALSE)
			return 0;
	}
	FillRemainder() ;
	return 1; 
}
