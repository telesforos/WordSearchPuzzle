//Programmer: James Moore
//Course: CS14

//File name:attempt.cpp /
//Project Name: Wordsrch

#include "attempt.h"

Rndm Attempt::RandGen; 

Attempt::Attempt (int TheWordIndex, int TheHeight,int TheWidth)
{
	Height = TheHeight;
	Width = TheWidth;
	WordIndex = TheWordIndex;
	NumPositions = Width * Height;
	PickFirstTry(); //to Complete initialization
}

void
Attempt::PickFirstTry()
{
	DirectionIndex = RandGen.UniformLong (0,NUM_DIRECTIONS-1) ;
	HorzStart = RandGen.UniformLong(1,Width);
	VertStart = RandGen.UniformLong (1,Height);

	DirectionAttempt = PositionAttempt = 1;
}

int
Attempt::NextTry ()
{
	if (NumPositions > PositionAttempt)
	{
		PositionAttempt++;
		HorzStart++;
		if (HorzStart > Width)
			HorzStart=1;

		if (HorzStart==1)
			VertStart++;
			
		if(VertStart > Height)
			VertStart = 1;
		return 1;

	}
	else if (DirectionAttempt < NUM_DIRECTIONS)
	{
		DirectionAttempt++;
		PositionAttempt = 1;
		DirectionIndex++;

		if (DirectionIndex == NUM_DIRECTIONS)
			DirectionIndex = 0;
		return 1;
	}
return 0;
}
