#include "wordlist.h"
#include <ostream>
#include <iomanip>

// Insertion sort. The lists are small (tens of words at most), so the simple
// algorithm is the right call; what matters is that it is a real sort rather
// than the previous selection-style double loop, which did the same number of
// comparisons but far more String assignments -- and each String assignment
// allocates.
template <class Compare>
static void
InsertionSort(WordList & List, Compare ShouldComeFirst)
{
	const int n = List.Length();

	for (int i = 1; i < n; ++i)
	{
		String Key = List[i];
		int j = i - 1;

		while (j >= 0 && ShouldComeFirst(Key, List[j]))
		{
			List[j + 1] = List[j];
			--j;
		}
		List[j + 1] = Key;
	}
}

// Sort words alphabetically (A -> Z)
void WordList::SortByAlpha()
{
	InsertionSort(*this, [](const String & A, const String & B)
							{ return A < B; });
}

// Sort words by size (LONGEST first)
void WordList::SortBySize()
{
	InsertionSort(*this, [](const String & A, const String & B)
							{ return A.Length() > B.Length(); });
}

int WordList::LongestWord() const
{
	int Longest = 0;
	for (int i = 0; i < Length(); ++i)
		if ((*this)[i].Length() > Longest)
			Longest = (*this)[i].Length();
	return Longest;
}

// Look for a repeated word. Reliable only on a sorted list, where duplicates
// must be adjacent -- which keeps this O(n) instead of O(n^2).
bool WordList::HasDuplicate(int & First, int & Second) const
{
	for (int i = 1; i < Length(); ++i)
		if ((*this)[i] == (*this)[i - 1])
		{
			First = i - 1;
			Second = i;
			return true;
		}
	return false;
}

// Output operator: title, blank line, then the words in numbered columns.
//
// The column count is derived the way the project specification describes it:
// work out how wide one "NN. LONGESTWORD" entry needs to be, then see how many
// of those fit evenly inside the width set by the puzzle grid. Entries are
// filled down each column in turn, so reading column one top to bottom gives
// items 1..RowCount.
std::ostream& operator<<(std::ostream& out, const WordList& rhs)
{
	out << rhs.Title << "\n\n";

	const int Count = rhs.Length();
	if (Count == 0)
		return out;

	// Width of the widest number label, e.g. 2 for "21".
	int NumberWidth = 1;
	for (int Value = Count; Value >= 10; Value /= 10)
		++NumberWidth;

	const int GAP = 2;   // blank columns between entries
	const int EntryWidth = NumberWidth + 2 + rhs.LongestWord() + GAP;

	int ColumnCount = rhs.Width / EntryWidth;
	if (ColumnCount < 1)
		ColumnCount = 1;
	if (ColumnCount > Count)
		ColumnCount = Count;

	// Round up so every entry lands in a column.
	const int RowCount = (Count + ColumnCount - 1) / ColumnCount;

	for (int Row = 0; Row < RowCount; ++Row)
	{
		for (int Col = 0; Col < ColumnCount; ++Col)
		{
			const int Index = Col * RowCount + Row;
			if (Index >= Count)
				break;

			const bool LastInRow =
				(Col == ColumnCount - 1) || (Index + RowCount >= Count);

			out << std::setw(NumberWidth) << (Index + 1) << ". ";

			if (LastInRow)
				out << rhs[Index].CStr();
			else
				out << std::left
					<< std::setw(rhs.LongestWord() + GAP)
					<< rhs[Index].CStr()
					<< std::right;
		}
		out << "\n";
	}

	return out;
}
