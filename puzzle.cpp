//Programmer: James Moore
//Course: CS14
//File name:puzzle.cpp

//Project Name: Wordsrch
#include "puzzle.h"
using namespace std;

const Puzzle::Offsets Puzzle::Directions [NUM_DIRECTIONS] =
//Directions 0 1 2 3 4 5 6 7

		{{0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}};

Rndm Puzzle::RandGen;

// How many times FillRemainder will re-roll the camouflage letters trying to
// avoid accidentally spelling a listed word a second time. A clean fill almost
// always happens on the first try; this bounds the rare bad case.
static const int MAX_FILL_ATTEMPTS = 20;

Puzzle::Puzzle(const Vector<String>& TheWords,int TheHeight,
int TheWidth):Words(TheWords)
{
	EXCEPTION(TheHeight <= 0 || TheWidth <= 0,
			  "Puzzle dimensions must be positive");

	Height = TheHeight;
	Width = TheWidth;

	NodeLimit = DEFAULT_NODE_LIMIT;
	Nodes = 0;
	Exhausted = false;

	AttemptStack = new Stack<Attempt>;

	// Allocate the rows one at a time, releasing whatever we already hold if a
	// later row fails, so a partly built grid is never leaked.
	Grid = 0;
	try
	{
		Grid = new LetterBox * [Height + 2];
		for (int i = 0; i < Height + 2; i++)
			Grid[i] = 0;
		for (int i = 0;i < Height+2;i++)
			Grid[i] = new LetterBox [Width + 2];
	}
	catch (...)
	{
		if (Grid)
		{
			for (int i = 0; i < Height + 2; i++)
				delete [] Grid[i];
			delete [] Grid;
		}
		delete AttemptStack;
		throw;
	}

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
			// The separator goes between letters, not after the last one, so a
			// row is exactly (Width * 2 - 1) characters wide.
			if (j > 1)
				Out << ' ';
			Out <<Rhs.Grid[i] [j].Letter;
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
			// Deliberately outside the else. Copies == 0 is how FillRemainder
			// recognises a camouflage cell after the letters have been written,
			// which is what lets it re-roll the fill. Wall cells are never
			// visited by RemoveOneWord, so a zero there is harmless.
			Grid[i] [j] .Copies = 0;
		}
	}

	// Design() may be called more than once; do not inherit a stale stack.
	AttemptStack->MakeEmpty();
}

bool
Puzzle::InsertOneWord(const Attempt & OneWord)
{
	int Row = OneWord.FirstVert();
	int Column = OneWord.FirstHorz();
	int Direction = OneWord.Direction();
	int WordIndex = OneWord.ID();

	Nodes++;

	for (int Ltr = 0;Ltr < Words[WordIndex].Length(); Ltr++)
	{
		// The WALL border guarantees this walk stops before leaving the grid:
		// '#' matches neither EMPTY nor any letter of a word.
		if(Grid [Row] [Column].Letter == EMPTY || Grid[Row] [Column].Letter==
			Words [WordIndex] [Ltr] )
			{
				Row = Row + Directions [Direction].Vert;
				Column = Column + Directions [Direction].Horz;
			}
			else
				return false;//failure to place word
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
	return true; //success
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

// Is Word the same read forwards and backwards? Such a word is found twice by
// CountOccurrences at a single location, once in each of two opposite
// directions, which is expected rather than an accidental duplicate.
static bool
IsPalindrome (const String & Word)
{
	for (int i = 0, j = Word.Length() - 1; i < j; i++, j--)
		if (Word[i] != Word[j])
			return false;
	return true;
}

int
Puzzle::CountOccurrences (const String & Word) const
{
	const int Len = Word.Length();
	if (Len == 0)
		return 0;

	int Total = 0;

	for (int Row = 1; Row < Height + 1; Row++)
		for (int Col = 1; Col < Width + 1; Col++)
			for (int Dir = 0; Dir < NUM_DIRECTIONS; Dir++)
			{
				int R = Row;
				int C = Col;
				bool Match = true;

				for (int Ltr = 0; Ltr < Len; Ltr++)
				{
					// As in InsertOneWord, the WALL border stops the walk.
					if (Grid[R][C].Letter != Word[Ltr])
					{
						Match = false;
						break;
					}
					R += Directions[Dir].Vert;
					C += Directions[Dir].Horz;
				}

				if (Match)
					Total++;
			}

	return Total;
}

bool
Puzzle::AllWordsUnique () const
{
	for (int i = 0; i < Words.Length(); i++)
	{
		// A single letter necessarily reads in all eight directions, so
		// uniqueness is not a meaningful question for it.
		if (Words[i].Length() < 2)
			continue;

		const int Expected = IsPalindrome(Words[i]) ? 2 : 1;
		if (CountOccurrences(Words[i]) != Expected)
			return false;
	}
	return true;
}

void
Puzzle::FillRemainder()
{
	// Camouflage letters can accidentally spell a listed word somewhere else in
	// the grid, giving the puzzle a second, unintended solution. Re-roll the
	// fill a bounded number of times to reach a grid where every word appears
	// exactly once.
	for (int Try = 0; Try < MAX_FILL_ATTEMPTS; Try++)
	{
		for(int i = 1;i < Height+1;i++)
		{
			for(int j = 1;j < Width+1;j++)
			{
				if( Grid[i] [j].Copies == 0)
					Grid[i] [j].Letter =char( RandGen.UniformLong('A','Z'));
			}
		}

		if (AllWordsUnique())
			return;
	}

	// Out of attempts. The grid is still a valid puzzle -- every word is
	// present and findable -- it just may contain one extra chance match.
}

bool
Puzzle::Design()
{
	Reset();
	Nodes = 0;
	Exhausted = false;

	if (Words.Length() == 0)
	{
		FillRemainder();
		return true;
	}

	// Initialised to true: on the first pass through the loop a successful
	// insertion leaves this untouched, and the backtracking loop below reads
	// it. The previous version left it uninitialised, so that read was
	// undefined behaviour.
	bool Tries = true;
	Attempt LocalAttempt (0, Height, Width);

	while(LocalAttempt.ID() < Words.Length())
	{
		// The search is complete -- NextTry() enumerates all
		// NUM_DIRECTIONS * Width * Height placements for each word -- but its
		// worst case is exponential in the number of words. Without this cap an
		// over-subscribed grid runs for hours with no output at all.
		if (NodeLimit > 0 && Nodes >= NodeLimit)
		{
			Exhausted = false;
			return false;
		}

		if (InsertOneWord (LocalAttempt) )
		{
			AttemptStack->Push (LocalAttempt) ;
			LocalAttempt .NextWord () ;
			Tries = true;
		}
		else
			Tries = LocalAttempt.NextTry() != 0;

		while (!Tries && AttemptStack->IsEmpty()==0)
		{
			LocalAttempt=AttemptStack->Top () ;
			AttemptStack->Pop() ;
			RemoveOneWord (LocalAttempt) ;
			Tries = LocalAttempt.NextTry() != 0;
		}

		if (!Tries)
		{
			// The first word ran out of positions and directions with nothing
			// left on the stack: every arrangement has been tried and failed.
			Exhausted = true;
			return false;
		}
	}
	FillRemainder() ;
	return true;
}
