#!/bin/sh
# End-to-end checks against the sample word lists.
#
# The unit tests in tests.cpp cannot reach the input validation, which lives in
# wordsrch.cpp behind main(). This script drives the real binary instead.
#
# The negative fixtures matter as much as the positive ones. laila.txt and
# serenity.txt contain 3RDGRADE and 6THGRADE and exist to prove the non-letter
# check fires; a change that lets them build a puzzle has broken validation,
# not fixed a bug.

set -u

BIN=./wordsrch
PASS=0
FAIL=0

run_puzzle()
{
	printf 'Fixture Test\n%s\nn\n' "$1" | $BIN 2>&1
}

# expect_ok FILE
expect_ok()
{
	Output=$(run_puzzle "$1")
	if printf '%s' "$Output" | grep -qiE 'error|gave up|unable'; then
		FAIL=$((FAIL + 1))
		echo "  FAIL $1 should have produced a puzzle, but reported:"
		printf '%s\n' "$Output" | grep -iE 'error|gave up|unable' | sed 's/^/        /'
	else
		PASS=$((PASS + 1))
		echo "  ok   $1 produced a puzzle"
	fi
}

# expect_error FILE EXPECTED_SUBSTRING
expect_error()
{
	Output=$(run_puzzle "$1")
	if printf '%s' "$Output" | grep -qF "$2"; then
		PASS=$((PASS + 1))
		echo "  ok   $1 was rejected as expected"
	else
		FAIL=$((FAIL + 1))
		echo "  FAIL $1 should have been rejected with: $2"
		printf '%s\n' "$Output" | tail -3 | sed 's/^/        /'
	fi
}

if [ ! -x "$BIN" ]; then
	echo "$BIN not built" >&2
	exit 1
fi

echo "Fixture tests"

echo " positive fixtures:"
expect_ok departments.txt
expect_ok garden.txt
expect_ok instruments.txt
expect_ok transport.txt
expect_ok valentine.txt

echo " negative fixtures (these MUST be rejected):"
expect_error laila.txt    'is not a letter'
expect_error serenity.txt 'is not a letter'

echo " generated negative cases:"

TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT

printf 'Too Long\n5 5\nENCYCLOPEDIA\nCAT\n'       > "$TMP/toolong.txt"
printf 'Duplicate\n8 8\nCAT\nDOG\nCAT\n'          > "$TMP/dup.txt"
printf 'Too Small\n1 1\nA\n'                      > "$TMP/tiny.txt"
printf 'Too Big\n99 99\nCAT\n'                    > "$TMP/huge.txt"
printf 'No Words\n8 8\n'                          > "$TMP/empty.txt"
printf '\n8 8\nCAT\n'                             > "$TMP/notitle.txt"
printf 'Bad Dimensions\nnot numbers\nCAT\n'       > "$TMP/baddim.txt"
printf 'Overpacked\n3 3\nABC\nDEF\nGHI\nJKL\n'    > "$TMP/packed.txt"

expect_error "$TMP/toolong.txt"  'cannot fit in a 5x5 grid'
expect_error "$TMP/dup.txt"      'appears more than once'
expect_error "$TMP/tiny.txt"     'too small'
expect_error "$TMP/huge.txt"     'too large'
expect_error "$TMP/empty.txt"    'No words found'
expect_error "$TMP/notitle.txt"  'Title line is empty'
expect_error "$TMP/baddim.txt"   'Cannot read grid dimensions'
expect_error "$TMP/packed.txt"   'cells'
expect_error "$TMP/nosuchfile.txt" 'Cannot open'

echo " session recovery:"
# A bad file must cost one prompt, not the whole session: the good file that
# follows five errors still has to build.
Output=$(printf 'Fixture Test\n%s\ny\n%s\ny\ngarden.txt\nn\n' \
	"$TMP/dup.txt" "$TMP/toolong.txt" | $BIN 2>&1)
if printf '%s' "$Output" | grep -q 'Words loaded: 12'; then
	PASS=$((PASS + 1))
	echo "  ok   session continued after two bad files"
else
	FAIL=$((FAIL + 1))
	echo "  FAIL session did not recover after bad input"
fi

echo
echo "$((PASS + FAIL)) fixture checks, $FAIL failures"
[ "$FAIL" -eq 0 ]
