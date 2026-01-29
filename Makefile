CXX      = g++
CXXFLAGS = -Wall -Wextra -std=c++17
TARGET   = wordsrch

SRCS = wordsrch.cpp puzzle.cpp wordlist.cpp string.cpp \
       Exception.cpp attempt.cpp stackar.cpp vector.cpp rndm.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Object file dependencies
wordsrch.o:  wordsrch.cpp Exception.h wordlist.h puzzle.h vector.h string.h stackar.h attempt.h absstack.h rndm.h
puzzle.o:    puzzle.cpp puzzle.h vector.h string.h stackar.h attempt.h Exception.h absstack.h rndm.h
wordlist.o:  wordlist.cpp wordlist.h vector.h string.h Exception.h
vector.o:    vector.cpp vector.h Exception.h
stackar.o:   stackar.cpp stackar.h absstack.h Exception.h
attempt.o:   attempt.cpp attempt.h rndm.h
Exception.o: Exception.cpp Exception.h
string.o:    string.cpp string.h
rndm.o:      rndm.cpp rndm.h

run: $(TARGET)
	./$(TARGET)
	
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
