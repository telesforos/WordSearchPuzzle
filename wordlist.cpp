#include "wordlist.h"
#include <ostream>

// Sort words alphabetically (A → Z)
void WordList::SortByAlpha()
{
    int n = Length();

    for (int i = 0; i < n - 1; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            if ((*this)[j] < (*this)[i])
            {
                String temp = (*this)[i];
                (*this)[i] = (*this)[j];
                (*this)[j] = temp;
            }
        }
    }
}

// Sort words by size (LONGEST first)
void WordList::SortBySize()
{
    int n = Length();

    for (int i = 0; i < n - 1; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            if ((*this)[j].Length() > (*this)[i].Length())
            {
                String temp = (*this)[i];
                (*this)[i] = (*this)[j];
                (*this)[j] = temp;
            }
        }
    }
}

// Output operator
std::ostream& operator<<(std::ostream& out, const WordList& rhs)
{
    out << rhs.Title << "\n\n";

    int currentWidth = 0;
    int count = rhs.Length();

    for (int i = 0; i < count; ++i)
    {
        const String& word = rhs[i];
        int wordLen = word.Length();

        // Wrap line if necessary
        if (currentWidth + wordLen + 1 > rhs.Width)
        {
            out << "\n";
            currentWidth = 0;
        }

        out << word << " ";
        currentWidth += wordLen + 1;
    }

    out << "\n";
    return out;
}
