# FOR_James: The WordSrch Review-and-Repair, Explained Over Coffee

So. You handed me a working word search generator and said "look at the design
and tell me what's wrong with it," and then "okay, fix it." Here's the whole
story — including the parts where I was wrong, because those are the useful
parts.

---

## Step 1: What I did first, and why

**I ran the thing before I read it properly.**

That sounds backwards. Most people open the files and start reading top to
bottom. I did skim the code first, but the moment I had a hypothesis, I went
looking for a way to *make the program misbehave in front of me*.

Here's the reasoning. Code review has two failure modes, and they pull in
opposite directions:

1. You read carefully, find twelve things that "look wrong," write them all up,
   and half of them turn out to be fine. Now you've wasted the reader's time and
   burned your credibility.
2. You read quickly, find the obvious stuff, and miss the thing that actually
   bites.

The antidote to both is the same: **make the bug show itself.** A bug you can
reproduce is a fact. A bug you reasoned your way to is a hypothesis wearing a
fact's clothing.

So my first three moves were:

- `make` — does it even build? (Yes, clean, no warnings. Noted: *no warnings*
  is itself information. It means `-Wall -Wextra` isn't catching whatever is
  wrong, so whatever is wrong is subtle.)
- Generate a deliberately impossible puzzle and time it. **It ran past 45
  seconds with a blank screen.** That's the headline.
- Try to compile one file from a different directory. **~30 errors about
  `memchr` not being declared.** That's finding number two, and I didn't
  predict it — I stumbled into it while trying to check something else.

That second one is worth dwelling on. I ran `g++ -I<projectdir>` from `/tmp`
because I wanted to check for `-O2`-only warnings. Instead I got a wall of
errors about the C string library. Why? Because your file was named
`string.h` (lower case), and `<cstring>` internally includes `<string.h>`, and
once your project directory is on the include path, *your* file wins. The
standard library couldn't find `memchr` because it was reading your String
class instead.

That's the kind of bug you cannot find by reading. It only exists in the
relationship between the filename, the include path, and the compiler's search
order.

**The lesson:** empirical probing finds a category of bug that reading never
will. Budget time for it.

---

## Step 2: The roads I didn't take (and the one I took and had to reverse)

### Road not taken #1: "Just rewrite it with std::string and std::vector"

This was genuinely tempting. Your `String` class has a `BufferLen == -1`
sentinel meaning "uninitialized," a `static char *NullString` that casts away
`const` from a string literal, reallocates on every single assignment, and had
an `operator[]` that returned a reference to a **shared mutable static byte**
when you indexed out of range. `std::string` has none of those problems and is
four hundred lines shorter.

I rejected it, and then I found the proof I was right to. When I grepped the
assignment spec (`theWordsearch.txt`) I found this at line 154 and around lines
479–481:

> "we will use a more sophisticated and powerful random number generator that
> will be provided called the Rndm class"
> "Dynamically allocate and store a new empty stack for attempt records"
> "Dynamically allocate a two dimensional puzzle grid"

The hand-rolled containers *are the assignment*. The whole point of CS14 Project
1 is to build the stack and feel the backtracking. Replacing them with the
standard library would be like handing in a store-bought cake at a baking class.

**This is the single most important judgment call in the whole job**, and I want
you to see the shape of it clearly: *the "better" engineering answer was the
wrong answer, because the code has a purpose beyond running correctly.* Code
exists inside a context — a course, a team convention, a regulatory constraint,
a customer's weird requirement. An improvement that ignores the context isn't an
improvement.

### Road not taken #2: The flat grid array

In my review I suggested replacing `LetterBox **Grid` (an array of pointers to
rows) with one flat `vector<LetterBox>` and index arithmetic. That's textbook
advice — better cache locality, one allocation instead of N+2, one `delete`
instead of N+2 chances to get it wrong.

Then I read the spec and found lines 479–481 explicitly requiring the
dynamically-allocated two-dimensional grid and the dynamically-allocated stack.

**So I withdrew the suggestion before writing a line of code.** I told you I was
withdrawing it and why. That matters more than being right the first time.

### The road I took and had to reverse: "The search isn't exhaustive"

Here's the mistake I actually made, and it's an instructive one.

In my review I claimed `Design()` wasn't a real depth-first search. My reasoning:
when the solver backtracks and then re-advances, `NextWord()` calls
`PickFirstTry()`, which **re-randomizes** the next word's starting position. I
looked at that and thought: *it's throwing away the knowledge of which
configurations already failed. It'll re-explore dead subtrees forever. The
"exhaustive search" claim in the header is a lie.*

It sounded right. It was wrong.

When I sat down to actually restructure the search, I had to trace `NextTry()`
line by line, and the picture changed:

```
if (NumPositions > PositionAttempt) {      // scan positions
    PositionAttempt++;
    HorzStart++;  if (HorzStart > Width) HorzStart = 1;
    if (HorzStart == 1) VertStart++;
    if (VertStart > Height) VertStart = 1;
    return 1;
} else if (DirectionAttempt < NUM_DIRECTIONS) {   // then rotate direction
    DirectionAttempt++;  PositionAttempt = 1;  DirectionIndex++;
    if (DirectionIndex == NUM_DIRECTIONS) DirectionIndex = 0;
    return 1;
}
return 0;
```

It's a raster scan **with wraparound**, counted by `PositionAttempt` up to
`NumPositions`. Starting from a random offset and wrapping means it still
touches every cell. Then it rotates through all 8 directions the same way. Total
coverage: exactly 8 × Width × Height, every combination, once.

The randomization changes the *order* of enumeration, not the *coverage*. And
under each parent placement, the child still exhausts its whole space before
reporting failure. It's a complete DFS. Lee Greenbank's design was right and my
critique was wrong.

I wrote a test to nail this down rather than just trusting my second reading —
`TestAttemptCoversWholeSearchSpace` allocates an array of 8×5×7 slots, runs
`NextTry()` to exhaustion, and asserts every slot was visited **exactly once**,
with zero missed and zero repeated. That test is now the load-bearing proof that
"no puzzle exists" is a truthful answer and not a shrug.

**Why did I get it wrong?** I pattern-matched. "Randomization inside a backtrack
loop" *usually* means an incomplete search, so I filled in the conclusion without
tracing the counter. The tell I ignored: the 5×5 impossible case had returned
"Unable to create the desired puzzle" **instantly and correctly** in my very
first probe. If the search were incomplete, that clean termination would have
been a coincidence. I had the disconfirming evidence in hand and skated past it.

**The lesson:** when the evidence and your theory disagree, the evidence is
usually right. And "this pattern usually means X" is a hypothesis generator, not
a conclusion.

The good news: getting this wrong changed the *fix*. If the search were
incomplete, the fix would have been restructuring it. Since it's complete, the
fix is a **node cap** — much smaller, much safer, and it preserves the
assignment's algorithm intact.

---

## Step 3: How the pieces fit together

The work has a deliberate order, and the order is load-bearing:

```
   Probe empirically  ──► Read the spec  ──► Fix bugs  ──► Build tests
        │                      │                │              │
   "what breaks?"      "what's allowed?"   "make it right"  "keep it right"
                              │
                              └──► withdrew 2 suggestions, corrected 1 claim
```

**Probe first** because it tells you where to aim. **Spec second** because it
tells you what you're *not allowed* to change — and it saved me from two wrong
turns and confirmed the `String.h` capital-S fix was the original design, not my
invention. **Fixes third.** **Tests last** — but not least; see below.

Within the fixes there's a dependency order too, and I didn't get to pick it:

1. `Exception.h` first — because everything else throws through it. Until the
   error strategy is settled, every other file is guessing about how to report
   problems.
2. Then the containers (`String`, `vector.h`, `stackar.h`) — because `Puzzle`
   and `WordList` are built on them.
3. Then `Puzzle` and `WordList`.
4. Then `wordsrch.cpp`, which orchestrates all of it.
5. Then the Makefile, then tests, then docs.

That's bottom-up, and it's not an accident. If I'd started at `wordsrch.cpp` I'd
have written code against interfaces I was about to change.

---

## Step 4: Tools and methods, and why these ones

**AddressSanitizer and UndefinedBehaviorSanitizer.** This is the single highest
leverage tool in the whole job, and it's one `make` target:

```make
asan: CXXFLAGS = -Wall -Wextra -std=c++17 -O1 -g -fsanitize=address,undefined
```

Here's the thing that should stick with you. Your build was clean under
`-Wall -Wextra`. Zero warnings. And yet `puzzle.cpp:143` had `int Tries;`
declared uninitialized and then **read** in a loop condition. That's undefined
behavior sitting in the middle of the main algorithm, and the compiler's warning
flags sailed right past it. (GCC's `-Wmaybe-uninitialized` needs optimization
on, and even then it's inconsistent about this shape.)

`-Wall -Wextra` is a spell-checker. The sanitizers are a proofreader who
actually reads for meaning. **Clean warnings prove nothing.**

**A hand-rolled test harness, not GoogleTest.** Forty lines of `CHECK` macro
versus a dependency. The project builds with nothing but `g++` and `make`, and
that's a real feature for a course project someone else has to compile. The
trade: no fixtures, no parameterized tests, no pretty output. For 1024 checks
across eleven test functions, I don't miss them.

**`-MMD -MP` for dependency generation.** Your Makefile had nine hand-written
dependency lines:

```make
puzzle.o: puzzle.cpp puzzle.h vector.h string.h stackar.h attempt.h ...
```

Those are correct *today*. They rot the instant someone adds an `#include`.
And the failure mode is vicious: `make` silently doesn't rebuild a file that
needed rebuilding, you get a stale object file, and you debug a bug you already
fixed. `-MMD -MP` has the compiler emit the dependencies as a side effect of
compiling — it cannot go stale, because it's generated by the thing that
actually knows.

**L'Ecuyer's combined generator with Schrage's method.** Your `Rndm` class
documented two seeds and used one. `Seed2` was stored, defended against being
zero, and then never read again — half the public interface was decoration. The
old core was also `Seed1 = (Seed1 * 16807) % 2147483647`, which overflows a
32-bit `long` (it happens to work on your machine because Linux `long` is 64-bit;
it would be UB on a 32-bit target).

I could have just deleted `Seed2`. I chose to make the implementation match its
documentation instead, because the header is the instructor-provided contract.
Schrage's method rewrites the multiply as `a*(s % q) - r*(s / q)` so no
intermediate ever leaves 32-bit range. Both seeds now do real work.

I also moved the Box-Muller spare deviate from function-level `static` to a
per-object member. As written, two `Rndm` objects would hand each other cached
values drawn from the wrong seed stream — a subtle cross-contamination bug that
nothing in this project triggers today but that would be miserable to find later.

---

## Step 5: The tradeoffs, both sides

**Node cap vs. purity.** The solver is a *complete* search — given forever, it
gives a definitive answer. I capped it at 200 million placement attempts. That
means the program can now say "I don't know" where before it would have said
(eventually, after hours) "definitely not."

I paid a real price: the program is now *less* capable of proving impossibility.
I bought: it always answers while you're still sitting there. That's the right
trade for an interactive tool, but I made it explicit rather than silent — the
two failure messages are different, and `SearchExhausted()` tells you which one
you got:

- *"every possible arrangement was tried and none fits"* — a proof.
- *"gave up after N placement attempts"* — a surrender.

**A cap that lies about which one it is would be worse than no cap at all.**

**Pre-validation vs. generality.** Rejecting a word longer than the grid takes
five lines and converts the worst user experience in the program into an instant,
specific error message. The cost: if someone ever wanted words to wrap around
the grid edges, this check would be in the way. That's a feature nobody asked
for, so I took the trade.

**Uniqueness re-rolling vs. speed.** `FillRemainder` now re-scans the whole grid
in all eight directions after filling, and re-rolls the camouflage if a random
letter accidentally spelled one of your words a second time. That's
`Height × Width × 8 × WordLength` character comparisons per word per attempt.
I capped it at 20 re-rolls. Almost always the first fill is clean, so the typical
cost is one scan. The cap means that in a pathological case you get a puzzle with
one extra chance match rather than a program that hangs — **I refused to fix a
hang by introducing a different hang.**

**Insertion sort vs. `std::sort`.** I kept a hand-written sort (the project is
about hand-written data structures) but changed it from your selection-style
double loop to insertion sort. Same comparison count, far fewer `String`
assignments — and every `String` assignment does a `new` and a `strcpy`. For 52
words in `departments.txt` this is invisible. I did it because the old loop
swapped three times per comparison in the worst case, and that's just gratuitous.

---

## Step 6: The mess — where I was wrong, in order

I've already covered the big one (the "not exhaustive" claim). Here are the
others, because the pattern across them is the actual lesson.

**Wrong #1: I claimed the puzzles were identical every run.** My reasoning: both
`Rndm` objects are static globals seeded from `clock()`, which is near zero at
static-init time, so both get the same seed and every run is the same puzzle.

I tested it before writing it down. Three runs, three different MD5 hashes.
`clock()` at static-init returns ~400–500 microseconds and varies enough
run-to-run. I wrote a five-line program to check the seeds directly:
`g1.seed1=532 g2.seed1=533`. Different, by one, every time.

So I downgraded the finding from "puzzles are deterministic" (false, alarming) to
"`Seed2` is dead code and the seeding is low-entropy" (true, minor). **The claim
I almost shipped would have been the loudest thing in the review and it was
false.**

**Wrong #2: my node-limit default was 12× too aggressive.** I picked 20 million
because it sounded like a big number. Then I benchmarked it:

```
5000000 nodes in 0.16 s = 31725102 nodes/sec
20M node default would take about 0.6 s
```

**Thirty-one million attempts per second.** My "generous" limit was six tenths of
a second of searching. A genuinely hard but *solvable* puzzle would have been
rejected with "gave up" almost instantly — I'd have replaced a hang with a liar.
Raised it to 200 million, about six seconds.

I only caught this because I measured instead of estimating. I had *no* intuition
for how fast that loop runs, and I'd have shipped a bad default on vibes.

**Wrong #3: my test fixtures were wrong twice.** First run of the suite: 4
failures out of 1024.

- I'd written `"ABCDE"` and `"FGHIJ"` in a 3×5 grid as my "provably impossible"
  case. It's trivially possible — two rows. I'd fixated on "5-letter words in a
  5-wide grid is tight" and forgotten there were three rows.
- My node-limit test built words from only 5 distinct letters, so they overlapped
  into an actual solution and the search *succeeded*.

Both were wrong *tests*, not wrong code. Which is its own lesson: when a test
fails, the test is a suspect too. I replaced the first with a case that's
impossible for a reason the search can prove cheaply (one 4-letter word in a
3×3 grid — no run is longer than 3), and asserted the exact node count: `8*3*3
= 72`. That's a much sharper test than "it returned false."

**Wrong #4 — and this is the best one, though not for the reason I first
thought: I disabled a working check to make a test fixture pass.**

I added a rule that words must be letters only. Then I ran all seven sample word
lists as a final check:

```
laila.txt     -> Error: Word "3RDGRADE" contains a character that is not a letter
serenity.txt  -> Error: Word "6THGRADE" contains a character that is not a letter
```

I read that as *"my improvement broke two of James's real word lists"* and
immediately relaxed the rule to allow digits. I even wrote a confident paragraph
in this document about how running against real data had saved me from shipping
a regression.

**It was the exact opposite.** You told me: `laila.txt` and `serenity.txt` are
*negative fixtures*. They contain `3RDGRADE` and `6THGRADE` precisely so that
someone running the program can watch the non-letter check fire. They are
supposed to fail.

So what actually happened is that my validation worked perfectly on the first
try, the test data proved it, and I responded by deleting the feature.

That is a genuinely bad failure mode, and it's worth naming precisely:

> **When existing data fails a new check, there are two possible stories: the
> check is wrong, or the data is a test case. I assumed the first without
> considering the second.**

The tell was right there and I walked past it. Two files, out of seven, both
failing the *same* check, both containing the *same* shape of input (a digit
prefix on a grade level). That's not the distribution of accidents — accidents
are scattered and varied. That's the distribution of *intent*. Real bad data
looks messy; deliberate bad data looks curated. Two clean, matched examples of
exactly one rule violation is a fixture, not a coincidence.

There's a second tell I also missed. I justified the relaxation by saying the
spec "only rules out embedded spaces." But I'd already read that this program
fills empty cells with random A–Z letters. A digit in a hidden word is therefore
**the only non-letter character on the entire grid** — the eye finds it
instantly and the puzzle is spoiled. The letters-only rule isn't pedantry, it
follows directly from how the camouflage works. I had every piece needed to work
that out and didn't assemble them, because I'd already decided the check was
wrong.

The fix: `isalpha` is back, with a comment in [wordsrch.cpp](wordsrch.cpp)
saying in plain language that those two files must keep failing. And because a
comment is only as good as the next person's willingness to read it, the
expectation is now executable — [fixtures_test.sh](fixtures_test.sh) runs the
real binary against all seven lists and **fails the build if `laila.txt` or
`serenity.txt` ever succeeds.** The knowledge you had to tell me is now
something the repository enforces on its own.

The lesson I originally drew from this — "test against real data" — wasn't
wrong, it was just shallow. The deeper one:

> **Data that fails is not automatically evidence of a bug. Find out what the
> data is *for* before you change code to accommodate it.** A test fixture and a
> regression look identical from inside the compiler. The difference lives in
> intent, and intent lives in someone's head or in a document — not in the
> bytes.

And the structural fix for that: **if a piece of data exists to prove a failure,
say so where the failure happens.** Unlabelled negative fixtures are a trap for
everyone who arrives later, and I walked straight into it.

There's a bonus finding buried in this one. When I narrowed `.gitignore` from
`*.txt` to `Puzzles.txt`, `git status` lit up with seven untracked files —
`garden.txt`, `instruments.txt`, all of them. **Those word lists had never been
committed.** The overly-broad ignore rule had been silently hiding the program's
own input data from version control this whole time. Clone the repo fresh and
the program has nothing to run on.

---

## Step 7: Pitfalls — the "I wish someone had told me" list

**Never name a header the same as a standard one.** `string.h`, `time.h`,
`stack.h`, `list.h`, `math.h`, `new.h`. Your project compiled fine for months
because you always ran `make` from the project root. The bug was real the entire
time; it was just asleep. The original spec named it `String.h` with a capital S
— case-sensitivity was the whole defense, and lowercasing it disarmed it.

**A clean `-Wall -Wextra` build means almost nothing.** Say it again. The
uninitialized `int Tries;` was a textbook undefined-behavior bug in the main
algorithm and the warnings were silent. Add `-fsanitize=address,undefined` as a
make target *today*, on every C++ project you own. It costs ten minutes.

**Never estimate performance. Measure it.** My 20-million default was off by
more than an order of magnitude in the *dangerous* direction. Thirty seconds
with `std::chrono` fixed it.

**Never let a program fail silently and slowly.** The original behavior for an
impossible puzzle was: blank screen, cursor blinking, forever. Every long
operation needs (a) a bound, (b) a distinct message for "impossible" vs. "gave
up," and ideally (c) progress output. The user cannot tell "working hard" from
"crashed" — and if they can't tell, you've failed them regardless of which it is.

**Validate cheaply before you compute expensively.** Checking `word.length() >
max(height, width)` costs microseconds and prevents a six-second search that was
never going to succeed. Cheap necessary conditions first, expensive search
second. This generalizes to nearly everything: check the file exists before
parsing it, check the array is non-empty before the O(n²) loop.

**`new` never returns NULL.** It throws `std::bad_alloc`. Your `vector.h` had
`if (Array == NULL) EXCEPTION(...)` — dead code inherited from C-era habits.

**Put the `try` where recovery is possible.** Yours wrapped the entire session,
so one typo'd filename killed the program. Moving it inside the loop means one
bad file costs one prompt. The scope of a `try` block *is* your recovery policy —
it's not a formality, it's a design decision.

**Rule of three, still.** `Puzzle` owned two raw allocations, had a destructor,
and had no copy constructor or copy assignment. Nothing copied a `Puzzle`
*today*. But the compiler will happily generate a copy that duplicates the
pointers and then frees them both. `= delete` costs two lines and makes it a
compile error instead of a crash.

---

## Step 8: What an expert notices that a beginner doesn't

**A beginner reads the code. An expert reads the code *and the requirements*.**
The two suggestions I withdrew (flat grid, by-value stack) were correct as
general C++ advice and wrong for this program. I only knew that because I
grepped a 68KB OCR'd scan of a 1997 assignment handout. Most reviewers wouldn't
have bothered. The context is part of the code.

**A beginner asks "is this correct?" An expert asks "how would I know?"** The
difference between "the search looks exhaustive to me" and a test that asserts
all 280 combinations are hit exactly once, with zero missed and zero repeated.
The first is an opinion. The second is a fact that stays true after the next
person edits the file.

**An expert distinguishes kinds of failure.** "Couldn't build the puzzle" is one
message. "I proved no arrangement exists" and "I ran out of patience" are two
completely different facts about the world, and the user needs different actions
for each. Collapsing them is a design failure, not a wording nitpick. The same
instinct shows up in the `SearchExhausted()` accessor — the *caller* gets to know
which happened.

**An expert notices what *isn't* there.** No tests. No sanitizer target. No
seed control — which meant no puzzle could ever be reproduced, which meant the
algorithm was effectively untestable. Adding `--seed` wasn't a user feature I
tacked on; it was the thing that made `TestPuzzleIsRepeatable` possible. Absences
are findings.

**An expert distrusts their own pattern-matching.** "Randomization in a backtrack
loop means incomplete search" is a good heuristic and it was wrong here. The
expert move isn't having better heuristics — it's noticing when the evidence
(that instant, correct 5×5 failure) contradicts the heuristic, and *going back*.

**An expert reads the accidental invariants.** Your `Reset()` had a TODO asking
whether `Copies = 0` belonged inside the `else` clause. The answer is no, and the
interesting part is *why*: `Copies == 0` is the only reliable way to tell a
camouflage cell from a word cell **after** the letters have been written — which
is exactly what lets `FillRemainder` re-roll the fill. A line you weren't sure
about turned out to be load-bearing for a feature that didn't exist yet. I put a
comment there explaining it, because the next person will wonder too.

---

## Step 9: What transfers to completely different work

**Reproduce before you diagnose.** True for code, true for a failing deploy,
true for "the client says the report looks wrong." The thing you can demonstrate
is worth ten things you can argue.

**Find out what you're not allowed to change, early.** Every project has
constraints that aren't in the artifact: a course requirement, a compliance rule,
a promise someone made to a customer, "the CEO likes the blue one." The spec grep
that saved me two wrong turns is the same move as asking "who else depends on
this?" before a refactor. **An improvement that violates a constraint is a
regression.**

**Distinguish "no" from "I don't know."** This is the deepest one here and it's
barely about programming. A search that gives up isn't the same as a search that
proved impossibility. A test that didn't run isn't a test that passed. "No data"
isn't "no effect." Systems that collapse these two — and *people* who collapse
these two — produce confident wrong answers. Every honest system needs a way to
say "I don't know."

**Measure the thing you're about to make a decision about.** I picked a limit
from intuition and was wrong by 12×. Thirty seconds of measurement fixed it.
Applies to database indexes, cache sizes, timeouts, staffing estimates, how long
a meeting should be.

**Before you accommodate failing data, find out what the data is for.** This is
the one I got wrong, and it generalizes far past code. A failing test, a customer
record that won't import, a row your script chokes on — the reflex is "my code is
too strict, loosen it." Sometimes right. But sometimes that record is the
*canary*, and loosening the rule kills the only thing that was watching. Ask what
the data is *for* before you change code to make it pass. And when the answer is
"it exists to fail," write that down where the failure happens — an unlabelled
negative fixture will catch the next person exactly as it caught me.

**Two similar failures are a pattern, not two accidents.** Real-world bad data
is scattered and varied. Two files failing the identical check with the
identical shape of input is a signature of intent. Learn to read the
*distribution* of failures, not just their content.

**Correct yourself in public and cheaply.** I got the "exhaustive search" claim
wrong, the determinism claim wrong, the node limit wrong, two test fixtures
wrong, and a validation rule wrong. The cost of each was small *because I found
them myself, quickly, by continuing to check.* The expensive version of being
wrong is the version where you defend it. Being wrong fast is a skill; the whole
game is shortening the loop between "I believe X" and "let me check X."

---

## The scoreboard

| | Before | After |
|---|---|---|
| Impossible puzzle | Hangs >45s, blank screen | Instant, specific error |
| Uninitialized read in `Design()` | Present (UB) | Fixed, initialized |
| Out-of-tree compile | ~30 errors | Clean |
| Unbounded `>>` into `char[256]` | 2 places | 0 |
| Error handling | `abort()` + `throw const char*` | `std::runtime_error` hierarchy |
| One bad file | Kills the session | Costs one prompt |
| Duplicate word solutions | Possible | Verified unique |
| Tests | 0 | 1024 unit checks + 17 fixture checks |
| Sanitizers | Not wired up | `make asan`, clean |
| Reproducible runs | Impossible | `--seed N` |
| Word lists in git | Hidden by `*.txt` | Tracked |
| Negative fixtures | Undocumented, easy to "fix" away | Enforced by the build |

Build is clean under `-Wall -Wextra -O2`. All 1024 unit checks pass under
AddressSanitizer and UndefinedBehaviorSanitizer. The five positive word lists
produce valid puzzles; `laila.txt` and `serenity.txt` are rejected, and the
build fails if they ever stop being rejected.

The backtracking algorithm — the actual assignment — is untouched, except that
it now stops when it's beaten and tells you honestly which kind of beaten it is.
