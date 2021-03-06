#include "Matrix.h"

Matrix::Matrix(AbstractCard cards[16]) {
	for (int i = 0; i < 16; i++) {
		itsCards[i] = cards[i];
	}
}

void Matrix::cellIsDone(int cellIndex) {
	AbstractCard DoneCard("D");
	itsCards[cellIndex] = DoneCard;
}

bool Matrix::isMatrixDone() {
	int lines[10][4] = { 0,1,2,3,	4,5,6,7,	8,9,10,11,	12,13,14,15,
						0,4,8,12,	1,5,9,13,	2,6,10,14,	3,7,11,15,
						0,5,10,15,	3,6,9,12 };
	for (int i = 0; i < 10; i++) {
		if (lineIsDone(lines[i][0], lines[i][1], lines[i][2], lines[i][3])) {
			return true;
		}
	}
	return false;
}
bool Matrix::lineIsDone(int cell1, int cell2, int cell3, int cell4) {
	if ((itsCards[cell1].getAbstractCard() == "D") && (itsCards[cell2].getAbstractCard() == "D") &&
		(itsCards[cell3].getAbstractCard() == "D") && (itsCards[cell4].getAbstractCard() == "D")) {
		return true;
	}
	else {
		return false;
	}
}

float Matrix::payout(int turnsLeft) {
	return payoutIfDone + (float) turnsLeft * rakeEconomyFor1Turn;
}