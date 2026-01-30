The user is prompted for the name of a text (.txt) file containing the list of words to be hidden in the Word Search puzzle. If the word file does not reside in the same directory as the WordSrch.exe program, then
the entire path name must be supplied.
The format of the word list text file is as follows:
1st line: Puzzle name - the entire first line of the file is used as the puzzle title.
2nd line: Height & Width- the second line provides two integers, separated by a space, specifying the puzzle height and width. Both integers must be greater than zero.
3rd line: Words - each remaining line of the puzzle provides one word to be hidden in the puzzle. Words can be any number of characters as long as they are not longer than the largest dimension of the puzzle. Words cannot contain embedded spaces.
After the file name has been entered, the puzzle is designed and displayed. The user is then prompted whether to create another puzzle or quit.
