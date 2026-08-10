// File Name: Exception.h
// Project Name: WordSrch
//
// Error reporting for the WordSrch project.
//
// All errors raised by this project derive from WordSearchError, which in turn
// derives from std::runtime_error. This means a single
// catch (const std::exception &) at the top level will catch everything,
// stack unwinding runs normally, and destructors get a chance to release
// memory. (The previous implementation called abort(), which did none of
// those things and could not be recovered from.)
//
// EXCEPTION(Condition, ErrMsg)
//   Precondition: none.
//   Postcondition: If Condition is non-zero, a RangeError carrying ErrMsg is
//   thrown. Otherwise returns normally. Retained under its original name so
//   the container templates can keep using it.

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>
#include <string>

// Base class for every error this project raises.
class WordSearchError : public std::runtime_error
{
public:
	explicit WordSearchError(const std::string & Msg)
		: std::runtime_error(Msg) {}
};

// The word list file could not be understood.
class FileFormatError : public WordSearchError
{
public:
	explicit FileFormatError(const std::string & Msg)
		: WordSearchError(Msg) {}
};

// A container was indexed or used outside its valid range.
class RangeError : public WordSearchError
{
public:
	explicit RangeError(const std::string & Msg)
		: WordSearchError(Msg) {}
};

// The word list is valid but cannot be turned into a puzzle.
class PuzzleError : public WordSearchError
{
public:
	explicit PuzzleError(const std::string & Msg)
		: WordSearchError(Msg) {}
};

void EXCEPTION(int Condition, const char *ErrMsg);

#endif
