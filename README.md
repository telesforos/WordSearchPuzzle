On execution the program ask for your name then you are prompted for the name of a text (.txt) file containing the list of words to be hidden in the Word Search puzzle. If the word file does not reside in the same directory as the WordSrch program, then
the entire path name must be supplied.
The format of the word list text file is as follows:
1st line: Puzzle name - the entire first line of the file is used as the puzzle title. The puzzle title is validated as being present.
2nd line: Height & Width- the second line provides two integers, separated by a space, specifying the puzzle height and width. Both integers must be greater than zero. There is a simple check on these values.
3rd line: Words - each remaining line of the puzzle provides one word to be hidden in the puzzle. Words can be any number of characters as long as they are not longer than the largest dimension of the puzzle. Words cannot contain embedded spaces.
After the file name has been entered, the puzzle is designed and displayed. The user is then prompted whether to create another puzzle or quit.
Output Specifications:
Once the puzzle is successfully designed it must be displayed on the screen and saved to a file. Only the puzzle letter grid is displayed on screen. Rows of the puzzle letter grid are single spaced; columns should have one space between letters.
The entire puzzle including the grid size, word count the letter grid and column word list is saved to a text file called Puzzles.txt. Following the puzzle grid the list of words is displayed in numbered columns, in alphabetical order, single spaced. The exact number of columns to use is computed by determining the number of columns of the largest word which fit evenly within the width set by the puzzle grid.
Puzzles.txt can then be viewed or printed from any word processor or text editor. The puzzle file should be viewed and printed using a monospaced font. such as Courier. If the program is unable to complete a puzzle from the given list of words, a simple error message is displayed on the screen.
