# Word Search Puzzle Generator - AI Coding Instructions

## Project Overview
A C++ application that generates word search puzzles. Users provide a word list and grid dimensions; the system places words in random directions (8-directional) and fills remaining cells with random letters. Complete puzzles are saved to output files.

## Architecture & Key Components

### Data Structures (Custom Implementations)
- **Vector<T>**: Generic bounds-checked template array with `operator[]`, `Length()`, `Resize()`, `Double()`
  - Located in [vector.h](vector.h) / [vector.cpp](vector.cpp)
  - Protected members allow `WordList` inheritance
  
- **String**: Custom string class with comparison operators, indexing, and char pointer conversion
  - Located in [string.h](string.h) / [string.cpp](string.cpp)
  - Supports `BufferLen = -1` marker for static null string
  
- **AbsStack<T>**: Abstract template interface for stack operations (Push, Pop, Top, IsEmpty, IsFull)
  - Located in [absstack.h](absstack.h) - implemented by `Stack<Attempt>`

### Core Classes

**Puzzle** ([puzzle.h](puzzle.h) / [puzzle.cpp](puzzle.cpp))
- Manages 2D grid with 1-indexed bounds (walls as padding at 0 and Height+2, Width+2)
- `Design()`: Main algorithm - attempts word insertion using backtracking via attempt stack
- `InsertOneWord()`: Places single word if no conflicts; tracks `Grid[i][j].Copies` for overlaps
- `Directions` array: 8 directions `{{0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1}, {1,-1}, {1,0}, {1,1}}`
- Grid uses `LetterBox` struct with `Letter` (char) and `Copies` (int) fields

**WordList** ([wordlist.h](wordlist.h) / [wordlist.cpp](wordlist.cpp))
- Inherits from `Vector<String>` with `Title` and `Width` metadata
- `SortBySize()`: Longest words first (improves placement odds for large words)
- Copy constructor disabled

**Attempt** ([attempt.h](attempt.h) / [attempt.cpp](attempt.cpp))
- Records word placement attempt: word index, start position (Vert/Horz), direction
- `NextTry()`: Iterates through position/direction combinations exhaustively
- Used in backtracking stack for puzzle design algorithm

**Exception** ([Exception.h](Exception.h) / [Exception.cpp](Exception.cpp))
- Macro `EXCEPTION(Condition, ErrMsg)` for error handling
- Used for file I/O validation and logic errors

### Supporting Classes
- **Rndm**: Random number generation (used by Puzzle for direction/position selection)
- **Stack<Attempt>**: Array-based stack implementation for backtracking

## Build & Development Workflow

### Build Commands
```bash
make CXXFLAGS="-Wall -Wextra -std=c++17 -g"  # Default with debug symbols
make clean                                     # Remove artifacts
```

### Compilation Details
- Standard: C++17
- Flags: `-Wall -Wextra` for strict warnings
- Object file dependencies listed in Makefile (all .cpp files compiled to .o)
- Single executable: `wordsrch`

### Input File Format
```
[Puzzle Title Line 1]
[Height] [Width]
[Word1]
[Word2]
...
```

### Output Files
- `Puzzles.txt`: Complete puzzle (grid + word list) saved with timestamp
- Puzzle output ordered by grid position, not input order

## Code Patterns & Conventions

### Memory Management
- C-style arrays with manual `new[]`/`delete[]` (not smart pointers)
- Destructor patterns: `delete AttemptStack; for() delete []Grid[i]; delete []Grid;`

### Grid Indexing
- Walls are sentinel boundaries: `Grid[0..Height+1][0..Width+1]`
- Valid puzzle area: `Grid[1..Height][1..Width]`
- Characters: `WALL = '#'`, `EMPTY = '.'`

### Exception Handling
- C-string exceptions: `throw "Unable to open log file!";`
- Try-catch in main; EXCEPTION macro for assertions

### Template Implementation
- All templates defined in headers ([vector.h](vector.h), [stackar.h](stackar.h), [absstack.h](absstack.h))
- Requires full class definitions for instantiation

### String Handling
- Mix of C strings (char[]) and custom `String` class
- `AllCaps()` function converts words to uppercase for consistency
- Input buffers sized at 256 chars

## Backtracking Algorithm Details

### Algorithm Overview
The `Design()` method in [puzzle.cpp](puzzle.cpp) implements depth-first backtracking to place words sequentially. For each word, it exhaustively tries all position/direction combinations until finding a valid placement or exhausting all options.

### Execution Flow

1. **Initial Setup**: Create `LocalAttempt` with `WordIndex=0`, random starting position and direction
2. **Main Loop**: While words remain to place (`LocalAttempt.ID() < Words.Length()`):
   - **Attempt Insertion**: Call `InsertOneWord(LocalAttempt)`
     - Validates placement by scanning forward: each cell must be `EMPTY` or match the word's letter
     - Does NOT modify grid during validation
   - **Success Path**: If placement valid:
     - Push `LocalAttempt` onto `AttemptStack` (backtrack point)
     - Call `LocalAttempt.NextWord()` to advance to next word with fresh random starting position
   - **Failure Path**: If placement invalid:
     - Call `LocalAttempt.NextTry()` to iterate next position/direction combo
     - If `NextTry()` returns true, continue main loop to retry placement
     - If `NextTry()` returns false, enter backtrack mode

3. **Backtrack Loop**: When no more position/direction combos exist:
   - While stack not empty AND no valid placement found:
     - Pop previous `LocalAttempt` from stack
     - Call `RemoveOneWord()` to undo its placement from grid
     - Call `NextTry()` on previous word to find alternate placement
   - If backtrack exhausts all previous words' alternatives, return failure (0)

4. **Success**: If all words placed, call `FillRemainder()` and return success (1)

### `Attempt::NextTry()` Iteration Scheme

Attempts positions and directions in nested loops:

**Position Loop** (NumPositions iterations):
- Increments horizontal start position: `HorzStart++` (wraps: `1..Width..1`)
- Increments vertical position after horizontal wraps: `VertStart++` (wraps: `1..Height..1`)
- Covers all grid cells starting from initial random position

**Direction Loop** (NUM_DIRECTIONS=8 iterations):
- After exhausting all positions, tries next direction (0..7)
- Resets position attempt counter to 1 and tries all positions in new direction
- Returns true while combinations remain, false when all exhausted

### Grid Modification Pattern

**InsertOneWord()**: 
- **Validation pass**: Scans forward, modifies nothing
- **If valid, insertion pass**: Writes letters to grid, increments `Grid[i][j].Copies` counter

**RemoveOneWord()**:
- Decrements `Copies` counter
- If `Copies > 1`, leaves letter (intersection with another word)
- If `Copies == 1`, clears to `EMPTY`
- Allows overlapping words at shared letters

### Key State Variables in `Attempt`

- `WordIndex`: Current word being placed (0..NumWords-1)
- `DirectionIndex`: Current direction (0..7)
- `VertStart`, `HorzStart`: Current position (1..Height/Width)
- `PositionAttempt`: Counter within position loop (1..NumPositions)
- `DirectionAttempt`: Counter within direction loop (1..NUM_DIRECTIONS)

## Critical Design Decisions

1. **Longest-words-first sorting**: Improves algorithm success rate by placing difficult words early
2. **8-directional placement**: Horizontal, vertical, and diagonal word orientations
3. **Exhaustive backtracking**: Stack-based attempt tracking ensures all position/direction combos tried
4. **Word overlap allowed**: Same letter can be reused at intersections (tracked by `Copies` counter)
5. **Sentinel boundaries**: 1-indexed grid with walls prevents bounds checking in placement logic

## Common Tasks

- **Adding new direction**: Modify `Directions` array in [puzzle.cpp](puzzle.cpp) and increment `NUM_DIRECTIONS`
- **Changing grid fill character**: Modify random char generation in `Design()` method
- **Adjusting word placement algorithm**: Core logic in `InsertOneWord()` and backtracking stack usage
- **Custom Vector behavior**: See protected members in [vector.h](vector.h) - modified from textbook for WordList inheritance

---

**Note**: This is an educational C++ project demonstrating data structures (custom Vector, Stack) and algorithmic problem-solving (backtracking puzzle generation).
