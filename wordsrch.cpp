// This project is used to design "Word Search” puzzles. The user enters
// the file name of a list ef words which are then randomly hidden in two
// Dimensional grid of characters. Unused positions in the grid are then
// filled with randomly generated characters to further camouflage the
// hidden words. The puzzle is displayed on screen without the word list 80
// that the results may be checked. The complete puzzle including the letter
// grid and the word list is written to a separate text file which may then
// LOCAL FUNCTIONS:
// int main( )
// Top level entry point to program. The executive.
// Void AllCaps(char Array[]);
// convert all lower case letters of a string to upper case in Array[].
// Precondition: Array[] contains a null terminated string.
// Postcondition: Each lower case letter in Array[] is now uppercase.
//
// void ValidateFileFormat(ifstream&, char*, int&, int&)
// Validates input file format and grid dimensions. Reads and validates the 
// puzzle title (line 1) and grid dimensions (line 2: HEIGHT WIDTH).
// Precondition: inputFile is an open input file stream positioned at the start.
// Title is a character buffer with capacity for at least 256 characters.
// Postcondition: Title contains the puzzle title. Height and Width contain
// validated dimensions (3-50 range). Throws descriptive error messages if
// format is invalid or dimensions are out of range.


#include <iostream>
#include <fstream>
#include "Exception.h"
#include <ctype.h>
#include "wordlist.h"
#include "puzzle.h"
using namespace std;

void AllCaps(char Array[]);
void ValidateFileFormat(ifstream& inputFile, char* Title, int& Height, int& Width);

int
main()
{
	try
	{
		// open log file
		ofstream LogFile("Puzzles.txt", ios::trunc);
		if (!LogFile)
			throw "Unable to open log file!";

		// Get the student's name and the date
		cout << "Enter your First and Last Name: ";
		char NameBuffer[50];
		cin.getline(NameBuffer, 50);
		LogFile << "Name : " << NameBuffer << endl;
		LogFile << "Project: Word Search" << endl;
		time_t TheTime;
		time (&TheTime);
		LogFile << "Date : " << dec << ctime(&TheTime) << endl;

		// Do as many puzzles as the user wants
		char Answer;
		do
		{
			// Get the name of the file containing the puzzle word list
			char Buffer[256];
			cout << "\nEnter the Word List filename: ";
			cin >> Buffer;

			// Open the word list input file
			ifstream WordFile (Buffer);
			if (WordFile)
			{
				// Validate file format and grid dimensions
				int Height, Width;
				char Title[256];
				ValidateFileFormat(WordFile, Title, Height, Width);
				
				cout << "\nPuzzle Title: " << Title << endl;
				cout << "Grid Size: " << Height << "x" << Width << endl;

				// Read and construct the word list from the remaining input file
				WordList TheList(20, Title, Width * 2);
				int WordCount = 0;
				char WordBuffer[256];
				while (WordFile >> WordBuffer)
				{
					AllCaps (WordBuffer);
					if (WordCount >= TheList.Length())
						TheList.Double();
					TheList [WordCount++] = WordBuffer;
				}
				
				if (WordCount == 0)
					throw "Error: No words found in file. Please ensure words are listed after the grid dimensions.";
				
				TheList.Resize (WordCount); // Shrink the list to fit
				cout << "Words loaded: " << WordCount << endl;
				TheList.SortBySize(); // Put longest words first

				// Construct, design, and display the puzzle
				Puzzle ThePuzzle(TheList, Height, Width);
				if (ThePuzzle.Design())
				{
					TheList.SortByAlpha();
					cout << endl << ThePuzzle << endl;
					LogFile << endl;
					for (int i = 0; i < Width * 2 - 1; i++)
						LogFile << '-';
					LogFile << endl << ThePuzzle << endl << TheList << endl;
				}
				else
					cerr << "Unable to create the desired puzzle.\n"
					<< "Try using a bigger puzzle grid, or fewer words."
					<< endl;
			}
			else
				cerr << "Cannot open " << Buffer << endl;

			// Does the user want to make another puzzle?
			cout << "Do you wish to make another puzzle? (y/N): ";
			cin >> Answer;
		}
		while (Answer == 'y' || Answer == 'Y');
	}
	catch (bad_alloc & Problem)
	{
		cerr << "Error: Memory allocation exception was raised." << endl;
	}
	catch (const char * Msg)
	{
		cerr << "Error: " << Msg << endl;
	}
	catch (...)
	{
	cerr << "Error: Unidentified exception was raised." << endl;
	}
	// Flush the input buffer -- the thorough way
	cin.clear();

	int CharsStillInBuffer = cin.rdbuf()->in_avail();
	cin.ignore(CharsStillInBuffer);

	// Done

	cout << "\nHit Enter to exit.";

	cin.ignore(200, '\n'); // Ignore everything till they hit Enter
	return 0;
}

void AllCaps(char Array[])
{
	for (int i = 0; Array[i]; i++)
		if (islower(Array[i]))
		Array[i] = char (toupper(Array[i]));
}

// Validates input file format: title line and grid dimensions
// Parameters:
//   inputFile: The input file stream positioned at the start
//   Title: Character buffer to store the puzzle title (must be at least 256 chars)
//   Height, Width: References to store validated grid dimensions
// Throws: descriptive error messages if format is invalid or dimensions are invalid
void ValidateFileFormat(ifstream& inputFile, char* Title, int& Height, int& Width)
{
	// Read and validate title line
	if (!inputFile.getline(Title, 256))
		throw "Error: Cannot read title line from file. File may be empty or malformed.";
	
	if (strlen(Title) == 0)
		throw "Error: Title line is empty. Please provide a puzzle title on the first line.";
	
	// Read and validate height/width line
	if (!(inputFile >> Height >> Width))
		throw "Error: Cannot read grid dimensions on second line. Expected format: HEIGHT WIDTH (two positive integers)";
	
	// Clear any remaining characters on the dimensions line (newline, extra whitespace)
	inputFile.ignore(256, '\n');
	
	// Validate dimension values
	if (Height <= 0 || Width <= 0)
		throw "Error: Grid dimensions must be positive integers (greater than 0).";
	
	if (Height < 3 || Width < 3)
		throw "Error: Grid dimensions too small. Minimum 3x3 grid required for puzzle.";
	
	if (Height > 50 || Width > 50)
		throw "Error: Grid dimensions too large. Maximum 50x50 grid supported.";
}