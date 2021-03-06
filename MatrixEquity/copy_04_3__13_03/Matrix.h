#pragma once

#include "AbstractCard.h"

class Matrix
{
public:
	Matrix(AbstractCard cards[16]);
	void cellIsDone(int cellIndex);
	bool isMatrixDone();

	AbstractCard itsCards[16];
	float payout(int turnsLeft);
private:
	bool lineIsDone(int cell1, int cell2, int cell3, int cell4);

	const float payoutIfDone = (float)15.64; // big blinds for bingo;
	const float rakeEconomyFor1Turn = (float)0.05;
	
};

