# WordSrch

Designs "Word Search" puzzles from a text file of words. Words are hidden at
random in a two-dimensional grid in any of eight directions and may overlap,
sharing letters. Leftover cells are filled with random letters as camouflage.

This is CS14 Project 1 (Greenbank). The backtracking search over an explicit
attempt stack is the point of the assignment.

## Building

```
make            # build ./wordsrch
make test       # build and run the unit tests
make asan       # run the tests under AddressSanitizer + UndefinedBehaviorSanitizer
make debug      # unoptimised build with debug symbols
make clean
```

## Running

```
./wordsrch [--seed N]
```

`--seed N` seeds both random number generators so a session can be reproduced
exactly. Without it, seeds come from the system clock and every run differs.

The program asks for your name, then for the name of a word list file. If the
file is not in the current directory, give the full path. Paths containing
spaces are fine. After each puzzle you are asked whether to make another.

An error in one word list only costs you that one prompt — the session
continues so you can try a different file.

## Word list file format

| Line | Contents |
| ---- | -------- |
| 1 | Puzzle title. The whole line is used. Must not be empty. |
| 2 | Height and width, two integers separated by a space. Each must be 3..50. |
| 3+ | One word per line. Letters only — no spaces, digits or punctuation. |

Words are converted to upper case. Digits and punctuation are rejected because
the camouflage fill only ever produces A–Z: a non-letter would be the only one
on the grid, and the eye lands straight on it.

The file is rejected, with a message naming the problem, if:

- a word is longer than the larger grid dimension (it could never fit);
- the words need more letters in total than the grid has cells;
- a word contains a character that is not a letter;
- the same word appears twice;
- the title is missing, or the dimensions are missing or out of range.

## Sample word lists

`departments.txt`, `garden.txt`, `instruments.txt`, `transport.txt` and
`valentine.txt` are positive samples — they should all produce puzzles.

**`laila.txt` and `serenity.txt` are negative fixtures.** They contain
`3RDGRADE` and `6THGRADE` and exist to prove the non-letter check fires. They
are *supposed* to be rejected. If a change makes them build a puzzle, that
change broke the validation.

These checks run before the search starts. Without them the solver would
disappear into an enormous search space to discover something that was
obvious up front.

## Output

The letter grid alone is printed to the screen, so the result can be checked
against the word list. Rows are single spaced and columns are separated by one
space.

The complete puzzle is written to `Puzzles.txt`, which is truncated at the start
of each run. For each puzzle it records the grid size, the word count, the
letter grid, and the word list in numbered columns in alphabetical order. The
number of columns is derived from how many entries the width of the longest
word allows within the printed width of the grid. View or print `Puzzles.txt`
with a monospaced font such as Courier.

Every word is guaranteed to appear in the finished grid exactly once: after
filling the empty cells with camouflage, the grid is re-scanned in all eight
directions and the fill is re-rolled if a random letter happened to spell a
listed word a second time.

## When a puzzle cannot be built

The search is complete — it enumerates all eight directions at every grid
position for every word — but its worst case is exponential. Two different
failures are reported differently:

- **Every arrangement was tried and none fits.** The search space was fully
  explored, so no puzzle exists for that word list and grid.
- **Gave up after N placement attempts.** The search hit its node budget
  (about six seconds). A puzzle might exist, but the grid is very crowded.

Either way, use a bigger grid or fewer words.

## Files

| File | Contents |
| ---- | -------- |
| `wordsrch.cpp` | Main program: input, validation, logging |
| `puzzle.cpp/.h` | The grid and the backtracking search |
| `attempt.cpp/.h` | One placement attempt; enumerates positions and directions |
| `wordlist.cpp/.h` | `Vector<String>` plus title, sorting and column output |
| `String.cpp/.h` | Dynamically allocated string. Capital S is deliberate — a lower-case `string.h` in this directory shadows the C standard header. |
| `vector.h` | Bounds-checked dynamic array template |
| `stackar.h`, `absstack.h` | Stack template and its abstract base |
| `rndm.cpp/.h` | L'Ecuyer combined pseudo-random generator |
| `Exception.cpp/.h` | Error types; `EXCEPTION()` throws rather than aborting |
| `tests.cpp` | Unit tests |
