// This project is used to design "Word Search" puzzles. The user enters
// the file name of a list of words which are then randomly hidden in a two
// dimensional grid of characters. Unused positions in the grid are then
// filled with randomly generated characters to further camouflage the
// hidden words. The puzzle is displayed on screen without the word list so
// that the results may be checked. The complete puzzle including the letter
// grid and the word list is written to a separate text file which may then
// be printed.
//
// USAGE: wordsrch [--seed N]
//   --seed N  Seed both random number generators with N so a session can be
//             reproduced exactly. Useful for testing and for regenerating a
//             puzzle you liked.
//
// LOCAL FUNCTIONS:
// int main(int argc, char *argv[])
//   Top level entry point to program. The executive.
//
// std::string AllCaps(const std::string & Text)
//   Precondition: none.
//   Postcondition: Returns Text with every lower case letter converted to
//   upper case.
//
// void ValidateFileFormat(std::ifstream &, std::string & Title, int & Height,
//                         int & Width)
//   Precondition: inputFile is an open input file stream positioned at the
//   start.
//   Postcondition: Title contains the puzzle title and Height and Width
//   contain validated dimensions (MIN_DIMENSION..MAX_DIMENSION). Throws
//   FileFormatError if the format is invalid or the dimensions are out
//   of range.
//
// void ValidateWordList(const WordList &, int WordCount, int Height, int Width)
//   Precondition: The list holds WordCount validated words.
//   Postcondition: Returns normally if the list could conceivably fit the
//   grid. Throws FileFormatError otherwise. This check exists because the
//   backtracking search cannot cheaply discover that a word is simply longer
//   than the grid, or that the words need more cells than the grid has; it
//   would instead grind through an enormous search space before failing.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include "Exception.h"
#include "wordlist.h"
#include "puzzle.h"
using namespace std;

// Grid dimension limits. Below 3 there is no room for a puzzle; the upper
// bound keeps one grid inside a printable page and bounds the search.
const int MIN_DIMENSION = 3;
const int MAX_DIMENSION = 50;

// Starting capacity of the word list. It doubles as needed.
const int INITIAL_WORD_CAPACITY = 20;

// Longest title line accepted.
const int MAX_TITLE_LENGTH = 255;

const char LOG_FILE_NAME[] = "Puzzles.txt";

static string AllCaps(const string & Text);
static void ValidateFileFormat(ifstream & inputFile, string & Title,
							   int & Height, int & Width);
static void ValidateWordList(const WordList & TheList, int WordCount,
							 int Height, int Width);
static void BuildOnePuzzle(const string & FileName, ostream & LogFile);
static bool AskYesNo(const char * Prompt);

int
main(int argc, char *argv[])
{
	// Optional reproducible seeding. Both generators are seeded from the same
	// value but offset, so they do not run in lockstep.
	for (int i = 1; i < argc; i++)
	{
		const string Arg = argv[i];
		if ((Arg == "--seed") && i + 1 < argc)
		{
			const long Seed = strtol(argv[++i], 0, 10);
			Puzzle::SeedRandom(Seed);
			Attempt::SeedRandom(Seed + 7919);
			cout << "Random seed: " << Seed << endl;
		}
		else
		{
			cerr << "Usage: " << argv[0] << " [--seed N]" << endl;
			return 1;
		}
	}

	// open log file
	ofstream LogFile(LOG_FILE_NAME, ios::trunc);
	if (!LogFile)
	{
		cerr << "Error: Unable to open " << LOG_FILE_NAME << " for writing."
			 << endl;
		return 1;
	}

	// Get the student's name and the date
	cout << "Enter your First and Last Name: ";
	string Name;
	getline(cin, Name);

	LogFile << "Name : " << Name << endl;
	LogFile << "Project: Word Search" << endl;
	time_t TheTime;
	time(&TheTime);
	LogFile << "Date : " << ctime(&TheTime);   // ctime supplies the newline

	// Do as many puzzles as the user wants
	do
	{
		// Get the name of the file containing the puzzle word list. getline
		// rather than >> so that paths containing spaces work.
		cout << "\nEnter the Word List filename: ";
		string FileName;
		if (!getline(cin, FileName))
			break;

		// The try sits inside the loop: one bad word list should cost the user
		// one prompt, not the whole session. Previously a single malformed file
		// ended the program.
		try
		{
			BuildOnePuzzle(FileName, LogFile);
		}
		catch (const bad_alloc &)
		{
			cerr << "Error: Ran out of memory building the puzzle." << endl;
		}
		catch (const std::exception & Problem)
		{
			cerr << "Error: " << Problem.what() << endl;
		}
		catch (...)
		{
			cerr << "Error: Unidentified exception was raised." << endl;
		}
	}
	while (AskYesNo("Do you wish to make another puzzle? (y/N): "));

	cout << "\nPuzzles written to " << LOG_FILE_NAME << "." << endl;
	return 0;
}

// Read a whole line and report whether it began with y or Y.
static bool
AskYesNo(const char * Prompt)
{
	cout << Prompt;
	string Answer;
	if (!getline(cin, Answer))
		return false;
	return !Answer.empty() && (Answer[0] == 'y' || Answer[0] == 'Y');
}

// Design, display and log a single puzzle from one word list file.
static void
BuildOnePuzzle(const string & FileName, ostream & LogFile)
{
	ifstream WordFile(FileName.c_str());
	if (!WordFile)
		throw FileFormatError("Cannot open " + FileName);

	// Validate file format and grid dimensions
	int Height, Width;
	string Title;
	ValidateFileFormat(WordFile, Title, Height, Width);

	cout << "\nPuzzle Title: " << Title << endl;
	cout << "Grid Size: " << Height << "x" << Width << endl;

	// Read and construct the word list from the remaining input file.
	// The print width matches one printed grid row, which is Width letters
	// separated by single spaces.
	const int PrintWidth = Width * 2 - 1;
	WordList TheList(INITIAL_WORD_CAPACITY, Title.c_str(), PrintWidth);

	int WordCount = 0;
	string Token;
	while (WordFile >> Token)          // std::string: no fixed buffer to overrun
	{
		const string Word = AllCaps(Token);

		// Reject anything that is not a plain word now, while we can still name
		// the offender, rather than letting it silently become part of a grid.
		//
		// Letters only. The camouflage fill only ever produces A..Z, so a digit
		// or a punctuation mark in a hidden word is a giveaway: it is the only
		// non-letter on the grid and the eye lands straight on it.
		//
		// laila.txt and serenity.txt are deliberate negative fixtures for this
		// check -- they contain 3RDGRADE and 6THGRADE and are SUPPOSED to be
		// rejected. Do not "fix" this rule to make them pass.
		for (string::size_type c = 0; c < Word.size(); c++)
			if (!isalpha(static_cast<unsigned char>(Word[c])))
				throw FileFormatError("Word \"" + Word + "\" contains a "
					"character that is not a letter. Words must be letters "
					"only, with no spaces, digits or punctuation.");

		if (WordCount >= TheList.Length())
			TheList.Double();
		TheList[WordCount++] = Word.c_str();
	}

	if (WordCount == 0)
		throw FileFormatError("No words found in file. Please ensure words are "
							  "listed after the grid dimensions.");

	TheList.Resize(WordCount); // Shrink the list to fit
	cout << "Words loaded: " << WordCount << endl;

	ValidateWordList(TheList, WordCount, Height, Width);

	// Duplicates are almost always a typo, and a repeated word makes the
	// "each word appears exactly once" guarantee impossible to state. Sorting
	// alphabetically first puts any duplicates next to each other.
	TheList.SortByAlpha();
	int FirstCopy, SecondCopy;
	if (TheList.HasDuplicate(FirstCopy, SecondCopy))
		throw FileFormatError(string("Word \"") + TheList[FirstCopy].CStr()
							  + "\" appears more than once in the list. "
								"Please remove the duplicate.");

	TheList.SortBySize(); // Put longest words first

	// Construct, design, and display the puzzle
	Puzzle ThePuzzle(TheList, Height, Width);

	if (!ThePuzzle.Design())
	{
		if (ThePuzzle.SearchExhausted())
			cerr << "Unable to create the desired puzzle: every possible "
					"arrangement was tried and none fits.\n"
					"Try using a bigger puzzle grid, or fewer words."
				 << endl;
		else
			cerr << "Gave up after " << ThePuzzle.NodesVisited()
				 << " placement attempts without finding an arrangement.\n"
					"The grid is very crowded. Try a bigger puzzle grid, or "
					"fewer words."
				 << endl;
		return;
	}

	TheList.SortByAlpha();

	// Only the letter grid goes to the screen, so the result can be checked
	// against the word list in the file.
	cout << endl << ThePuzzle << endl;

	LogFile << endl;
	for (int i = 0; i < PrintWidth; i++)
		LogFile << '-';
	LogFile << endl
			<< "Grid Size: " << Height << " x " << Width
			<< "    Words: " << WordCount << endl
			<< endl
			<< ThePuzzle << endl
			<< TheList << endl;
}

static string
AllCaps(const string & Text)
{
	string Result = Text;
	for (string::size_type i = 0; i < Result.size(); i++)
		Result[i] = char(toupper(static_cast<unsigned char>(Result[i])));
	return Result;
}

// Validates input file format: title line and grid dimensions
static void
ValidateFileFormat(ifstream & inputFile, string & Title, int & Height,
				   int & Width)
{
	// Read and validate title line
	if (!getline(inputFile, Title))
		throw FileFormatError("Cannot read title line from file. File may be "
							  "empty or malformed.");

	// Tolerate files saved with Windows line endings.
	if (!Title.empty() && Title[Title.size() - 1] == '\r')
		Title.erase(Title.size() - 1);

	if (Title.empty())
		throw FileFormatError("Title line is empty. Please provide a puzzle "
							  "title on the first line.");

	if (Title.size() > (string::size_type)MAX_TITLE_LENGTH)
		throw FileFormatError("Title line is too long.");

	// Read and validate height/width line. Read the line first, then parse it,
	// so that "10 10 extra" is reported rather than silently accepted.
	string DimensionLine;
	if (!getline(inputFile, DimensionLine))
		throw FileFormatError("Cannot read grid dimensions on second line. "
							  "Expected format: HEIGHT WIDTH (two positive "
							  "integers)");

	istringstream Dimensions(DimensionLine);
	string Trailing;
	if (!(Dimensions >> Height >> Width) || (Dimensions >> Trailing))
		throw FileFormatError("Cannot read grid dimensions on second line. "
							  "Expected format: HEIGHT WIDTH (two positive "
							  "integers)");

	// Validate dimension values
	if (Height <= 0 || Width <= 0)
		throw FileFormatError("Grid dimensions must be positive integers "
							  "(greater than 0).");

	if (Height < MIN_DIMENSION || Width < MIN_DIMENSION)
	{
		ostringstream Msg;
		Msg << "Grid dimensions too small. Minimum "
			<< MIN_DIMENSION << "x" << MIN_DIMENSION
			<< " grid required for puzzle.";
		throw FileFormatError(Msg.str());
	}

	if (Height > MAX_DIMENSION || Width > MAX_DIMENSION)
	{
		ostringstream Msg;
		Msg << "Grid dimensions too large. Maximum "
			<< MAX_DIMENSION << "x" << MAX_DIMENSION << " grid supported.";
		throw FileFormatError(Msg.str());
	}
}

// Cheap impossibility checks. Each of these would otherwise send the
// backtracking search off on a hunt it cannot possibly win.
static void
ValidateWordList(const WordList & TheList, int WordCount, int Height, int Width)
{
	const int LongestAllowed = (Height > Width) ? Height : Width;

	long TotalLetters = 0;
	for (int i = 0; i < WordCount; i++)
	{
		const int Len = TheList[i].Length();
		TotalLetters += Len;

		if (Len > LongestAllowed)
		{
			ostringstream Msg;
			Msg << "Word \"" << TheList[i].CStr() << "\" is "
				<< Len << " letters, which cannot fit in a "
				<< Height << "x" << Width << " grid (longest run is "
				<< LongestAllowed << "). Use a bigger grid or a shorter word.";
			throw FileFormatError(Msg.str());
		}
	}

	// Words may share letters, so this is a necessary condition rather than a
	// sufficient one, but it catches wildly over-subscribed grids instantly.
	const long Capacity = (long)Height * Width;
	if (TotalLetters > Capacity)
	{
		ostringstream Msg;
		Msg << "These " << WordCount << " words need " << TotalLetters
			<< " letters but a " << Height << "x" << Width
			<< " grid has only " << Capacity
			<< " cells. Use a bigger grid or fewer words.";
		throw FileFormatError(Msg.str());
	}
}
