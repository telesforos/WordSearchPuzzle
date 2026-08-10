// File Name: tests.cpp
// Project Name: WordSrch
//
// A small self-contained test harness. No external framework: the project
// builds with nothing but g++ and make, and adding a dependency just to get
// assertion macros would not be a fair trade.
//
// Run with:  make test
// Run under the sanitizers with:  make asan

#include <iostream>
#include <sstream>
#include <string>
#include "String.h"
#include "vector.h"
#include "stackar.h"
#include "wordlist.h"
#include "attempt.h"
#include "puzzle.h"
#include "rndm.h"

static int Checks = 0;
static int Failures = 0;

#define CHECK(Condition)                                                    \
	do {                                                                    \
		Checks++;                                                           \
		if (!(Condition))                                                   \
		{                                                                   \
			Failures++;                                                     \
			std::cout << "  FAIL " << __FILE__ << ":" << __LINE__           \
					  << "  " << #Condition << std::endl;                   \
		}                                                                   \
	} while (0)

// Assert that Body throws ExceptionType.
#define CHECK_THROWS(ExceptionType, Body)                                   \
	do {                                                                    \
		Checks++;                                                           \
		bool Caught = false;                                                \
		try { Body; }                                                       \
		catch (const ExceptionType &) { Caught = true; }                    \
		catch (...) { }                                                     \
		if (!Caught)                                                        \
		{                                                                   \
			Failures++;                                                     \
			std::cout << "  FAIL " << __FILE__ << ":" << __LINE__           \
					  << "  expected " << #ExceptionType                    \
					  << " from " << #Body << std::endl;                    \
		}                                                                   \
	} while (0)

static void
TestString()
{
	std::cout << "String" << std::endl;

	String Empty;
	CHECK(Empty.Length() == 0);

	String A("HELLO");
	CHECK(A.Length() == 5);
	CHECK(A[0] == 'H');
	CHECK(A[4] == 'O');

	// Out of range indexing must be reported, not silently absorbed. The old
	// implementation returned '\0' / a shared static char.
	CHECK_THROWS(RangeError, A[5]);
	CHECK_THROWS(RangeError, A[-1]);

	String B(A);
	CHECK(B == A);
	B = "WORLD";
	CHECK(B != A);
	CHECK(A < B);
	CHECK(B > A);

	// Self assignment must not corrupt the buffer.
	A = A;
	CHECK(A == String("HELLO"));

	String C;
	C = 'X';
	CHECK(C.Length() == 1);
	CHECK(C[0] == 'X');
	C += 'Y';
	CHECK(C.Length() == 2);
	CHECK(C == String("XY"));

	// operator>> must survive a token longer than any fixed buffer. The old
	// version read into char[256] with an unbounded extraction.
	const std::string Long(5000, 'Q');
	std::istringstream In(Long);
	String Read;
	In >> Read;
	CHECK(Read.Length() == 5000);
	CHECK(std::string(Read.CStr()) == Long);
}

static void
TestVector()
{
	std::cout << "Vector" << std::endl;

	Vector<int> V(3);
	CHECK(V.Length() == 3);
	V[0] = 10; V[1] = 20; V[2] = 30;

	CHECK_THROWS(RangeError, V[3]);
	CHECK_THROWS(RangeError, V[-1]);

	V.Resize(5);
	CHECK(V.Length() == 5);
	CHECK(V[0] == 10);
	CHECK(V[2] == 30);

	V.Double();
	CHECK(V.Length() == 10);
	CHECK(V[1] == 20);

	V.Resize(2);
	CHECK(V.Length() == 2);
	CHECK(V[1] == 20);

	CHECK_THROWS(RangeError, Vector<int> Bad(-1));
}

static void
TestStack()
{
	std::cout << "Stack" << std::endl;

	Stack<int> S;
	CHECK(S.IsEmpty());
	CHECK_THROWS(RangeError, S.Top());
	CHECK_THROWS(RangeError, S.Pop());

	// Push past the initial capacity of 10 to exercise the growth path.
	for (int i = 0; i < 100; i++)
		S.Push(i);

	CHECK(!S.IsEmpty());
	CHECK(S.Top() == 99);

	for (int i = 99; i >= 0; i--)
	{
		CHECK(S.Top() == i);
		S.Pop();
	}
	CHECK(S.IsEmpty());

	S.Push(1);
	S.MakeEmpty();
	CHECK(S.IsEmpty());
}

static void
TestRndm()
{
	std::cout << "Rndm" << std::endl;

	Rndm Gen(12345, 67890);

	// Check the ranges over many draws, but report at most one failure so a
	// broken generator does not print 20000 lines.
	bool RealInRange = true;
	bool LongInRange = true;
	bool SawLow = false, SawHigh = false;

	for (int i = 0; i < 20000; i++)
	{
		const double U = Gen.UniformReal();
		if (!(U > 0.0 && U < 1.0))
			RealInRange = false;

		const long L = Gen.UniformLong(1, 10);
		if (L < 1 || L > 10)
			LongInRange = false;
		if (L == 1)  SawLow = true;
		if (L == 10) SawHigh = true;
	}

	CHECK(RealInRange);
	CHECK(LongInRange);
	// Both endpoints must be reachable: UniformLong is documented as inclusive.
	CHECK(SawLow);
	CHECK(SawHigh);

	// Same seeds must reproduce the same stream, or --seed is worthless.
	Rndm A(42, 99);
	Rndm B(42, 99);
	bool Same = true;
	for (int i = 0; i < 1000; i++)
		if (A.UniformReal() != B.UniformReal())
			Same = false;
	CHECK(Same);

	// Different seeds must not.
	Rndm C(43, 99);
	Rndm D(42, 99);
	CHECK(C.UniformReal() != D.UniformReal());

	// A zero seed used to be silently rewritten to 1; make sure it is at least
	// still a working generator and not a fixed point.
	Rndm Zero(0, 0);
	CHECK(Zero.UniformReal() != Zero.UniformReal());

	// UniformLong on a degenerate range.
	CHECK(Gen.UniformLong(7, 7) == 7);
}

static void
TestAttemptCoversWholeSearchSpace()
{
	std::cout << "Attempt: search space coverage" << std::endl;

	// NextTry() must enumerate every (direction, row, column) triple exactly
	// once before reporting that it is out of options. This is the property the
	// whole backtracking search rests on: if it were not true, a "no puzzle
	// exists" answer would be a lie.
	const int H = 5, W = 7;
	Attempt A(0, H, W);

	const int Total = 8 * H * W;
	Vector<int> Seen(Total);
	for (int i = 0; i < Total; i++)
		Seen[i] = 0;

	int Count = 1;
	int Index = A.Direction() * H * W + (A.FirstVert() - 1) * W
				+ (A.FirstHorz() - 1);
	Seen[Index] = 1;

	while (A.NextTry())
	{
		Count++;
		if (Count > Total)
			break;   // guard against a runaway loop in a broken implementation

		CHECK(A.Direction() >= 0 && A.Direction() < 8);
		CHECK(A.FirstVert() >= 1 && A.FirstVert() <= H);
		CHECK(A.FirstHorz() >= 1 && A.FirstHorz() <= W);

		Index = A.Direction() * H * W + (A.FirstVert() - 1) * W
				+ (A.FirstHorz() - 1);
		Seen[Index]++;
	}

	CHECK(Count == Total);

	int Missed = 0, Repeated = 0;
	for (int i = 0; i < Total; i++)
	{
		if (Seen[i] == 0) Missed++;
		if (Seen[i] > 1)  Repeated++;
	}
	CHECK(Missed == 0);
	CHECK(Repeated == 0);
}

// Count how many times Needle occurs in Haystack.
static int
CountSubstring(const std::string & Haystack, const std::string & Needle)
{
	int Total = 0;
	std::string::size_type Pos = 0;
	while ((Pos = Haystack.find(Needle, Pos)) != std::string::npos)
	{
		Total++;
		Pos++;
	}
	return Total;
}

static void
TestPuzzleSucceeds()
{
	std::cout << "Puzzle: solvable grid" << std::endl;

	Puzzle::SeedRandom(2024);
	Attempt::SeedRandom(1999);

	const char *Words[] = {"CARROT", "CELERY", "BEANS", "PEAS", "BEET"};
	const int N = 5;

	Vector<String> List(N);
	for (int i = 0; i < N; i++)
		List[i] = Words[i];

	Puzzle P(List, 10, 10);
	CHECK(P.Design());

	// Every word must be present, and present exactly once -- the camouflage
	// fill must not accidentally spell one a second time.
	for (int i = 0; i < N; i++)
		CHECK(P.CountOccurrences(List[i]) == 1);

	// The finished grid must be all capital letters: no EMPTY cells left over
	// and no WALL leaking into the printed area.
	std::ostringstream Out;
	Out << P;
	const std::string Text = Out.str();

	int Rows = 0;
	bool AllLetters = true;
	for (std::string::size_type i = 0; i < Text.size(); i++)
	{
		const char Ch = Text[i];
		if (Ch == '\n') { Rows++; continue; }
		if (Ch == ' ')  continue;              // column separator
		if (Ch < 'A' || Ch > 'Z') AllLetters = false;
	}
	CHECK(AllLetters);
	CHECK(Rows == 10);

	// Each printed row is Width letters separated by single spaces.
	CHECK(CountSubstring(Text, "\n") == 10);
}

static void
TestPuzzleProvesImpossible()
{
	std::cout << "Puzzle: unsatisfiable grid" << std::endl;

	Puzzle::SeedRandom(7);
	Attempt::SeedRandom(7);

	// A single word longer than the longest run the grid offers. This is the
	// one shape of impossibility the search can prove cheaply: the first word
	// fails in all 8 * Height * Width placements and there is nothing on the
	// stack to backtrack into.
	Vector<String> List(1);
	List[0] = "ABCD";          // 4 letters; the longest run in 3x3 is 3

	Puzzle P(List, 3, 3);
	CHECK(!P.Design());

	// It must report that it *proved* impossibility rather than gave up.
	CHECK(P.SearchExhausted());

	// Exactly one attempt per (direction, row, column), and no more.
	CHECK(P.NodesVisited() == 8 * 3 * 3);
}

static void
TestPuzzleRespectsNodeLimit()
{
	std::cout << "Puzzle: node limit" << std::endl;

	Puzzle::SeedRandom(11);
	Attempt::SeedRandom(11);

	// 25 fifteen-letter words need 375 cells; a 15x15 grid has 225. This is the
	// case that used to run forever with no output. With a node limit it now
	// returns promptly and says which kind of failure it was.
	// Draw from a small alphabet of letters chosen so the words look random
	// rather than regular; regular words overlap so freely that the grid turns
	// out to be solvable, which is not what this test is about.
	const char Alphabet[] = "QXZJVKW";
	unsigned long R = 1;

	const int N = 25;
	Vector<String> List(N);
	for (int i = 0; i < N; i++)
	{
		String W;
		for (int j = 0; j < 15; j++)
		{
			R = R * 1103515245UL + 12345UL;
			W += Alphabet[(R >> 16) % 7];
		}
		List[i] = W;
	}

	Puzzle P(List, 15, 15);
	P.SetNodeLimit(50000);

	CHECK(!P.Design());
	CHECK(!P.SearchExhausted());          // gave up, did not prove impossible
	CHECK(P.NodesVisited() >= 50000);
	CHECK(P.NodesVisited() < 50000 + 100); // stopped promptly after the limit
}

static void
TestPuzzleIsRepeatable()
{
	std::cout << "Puzzle: Design() is repeatable and re-runnable" << std::endl;

	Vector<String> List(3);
	List[0] = "ALPHA";
	List[1] = "BETA";
	List[2] = "GAMMA";

	Puzzle::SeedRandom(555);
	Attempt::SeedRandom(556);
	Puzzle P1(List, 8, 8);
	CHECK(P1.Design());
	std::ostringstream First;
	First << P1;

	Puzzle::SeedRandom(555);
	Attempt::SeedRandom(556);
	Puzzle P2(List, 8, 8);
	CHECK(P2.Design());
	std::ostringstream Second;
	Second << P2;

	// Same seeds, same puzzle. This is what makes --seed and these tests work.
	CHECK(First.str() == Second.str());

	// Design() resets the grid, so calling it twice must still give a valid
	// puzzle rather than one polluted by the previous run's letters.
	CHECK(P1.Design());
	for (int i = 0; i < 3; i++)
		CHECK(P1.CountOccurrences(List[i]) == 1);
}

static void
TestPuzzleRejectsBadDimensions()
{
	std::cout << "Puzzle: bad dimensions" << std::endl;

	Vector<String> List(1);
	List[0] = "HI";

	CHECK_THROWS(RangeError, Puzzle P(List, 0, 5));
	CHECK_THROWS(RangeError, Puzzle P(List, 5, -1));
}

static void
TestWordList()
{
	std::cout << "WordList" << std::endl;

	WordList L(4, "Vegetables", 63);
	L[0] = "PEAS";
	L[1] = "CARROT";
	L[2] = "BEET";
	L[3] = "CELERY";

	L.SortBySize();
	CHECK(L[0].Length() >= L[1].Length());
	CHECK(L[1].Length() >= L[2].Length());
	CHECK(L[2].Length() >= L[3].Length());
	CHECK(L.LongestWord() == 6);

	L.SortByAlpha();
	CHECK(L[0] == String("BEET"));
	CHECK(L[1] == String("CARROT"));
	CHECK(L[2] == String("CELERY"));
	CHECK(L[3] == String("PEAS"));

	int A, B;
	CHECK(!L.HasDuplicate(A, B));

	L[2] = "CARROT";
	L.SortByAlpha();
	CHECK(L.HasDuplicate(A, B));
	CHECK(L[A] == L[B]);

	// Numbered column output, as specified by the project handout.
	WordList M(3, "Trio", 63);
	M[0] = "ALPHA";
	M[1] = "BETA";
	M[2] = "GAMMA";
	std::ostringstream Out;
	Out << M;
	const std::string Text = Out.str();

	CHECK(Text.find("Trio") == 0);
	CHECK(Text.find("1. ALPHA") != std::string::npos);
	CHECK(Text.find("2. BETA") != std::string::npos);
	CHECK(Text.find("3. GAMMA") != std::string::npos);

	// A narrow print width must fall back to a single column rather than
	// dividing by zero or producing negative column counts.
	WordList Narrow(2, "Narrow", 1);
	Narrow[0] = "LONGWORDHERE";
	Narrow[1] = "ANOTHERLONGWORD";
	std::ostringstream NarrowOut;
	NarrowOut << Narrow;
	CHECK(CountSubstring(NarrowOut.str(), "\n") == 4);  // title, blank, 2 rows
}

int
main()
{
	std::cout << "WordSrch unit tests" << std::endl << std::endl;

	TestString();
	TestVector();
	TestStack();
	TestRndm();
	TestAttemptCoversWholeSearchSpace();
	TestWordList();
	TestPuzzleSucceeds();
	TestPuzzleProvesImpossible();
	TestPuzzleRespectsNodeLimit();
	TestPuzzleIsRepeatable();
	TestPuzzleRejectsBadDimensions();

	std::cout << std::endl
			  << Checks << " checks, " << Failures << " failures" << std::endl;

	return Failures == 0 ? 0 : 1;
}
