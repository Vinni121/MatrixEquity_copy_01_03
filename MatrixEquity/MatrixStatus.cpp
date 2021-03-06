#include "MatrixStatus.h"


MatrixStatus::MatrixStatus(bool inputClosedCells[16]) {
	closedCellsAsInt = 0;
	for (int i = 0; i < 16; i++) {
		closedCells[i] = inputClosedCells[i];
		closedCellsAsInt *= 2;
		closedCellsAsInt += closedCells[i];
	}	
}

MatrixStatus::MatrixStatus(int inputClosedCellsAsInt) {
	for (int i = 15; i >= 0; --i) {
		closedCells[i] = inputClosedCellsAsInt & 1;
		inputClosedCellsAsInt = inputClosedCellsAsInt >> 1;
	}
	closedCellsAsInt = inputClosedCellsAsInt;
}

MatrixStatus::MatrixStatus() {
	for (int i = 0; i < 16; i++) {
		closedCells[i] = false;
	}
}

void MatrixStatus::closeC(int cellIndex) {
	closedCells[cellIndex] = true;
}

bool MatrixStatus::isMatrixDone() {
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
bool MatrixStatus::lineIsDone(int cell1, int cell2, int cell3, int cell4) {
	return (closedCells[cell1] && closedCells[cell2] && closedCells[cell3] && closedCells[cell4]);
}

float MatrixStatus::payout(int turnsLeft) {
	return payoutIfDone + (float)turnsLeft * rakeEconomyFor1Turn;
}

int MatrixStatus::numClosedCells() {
	int closedCount = 0;
	for (int i = 0; i < 16; i++) {
		if (closedCells[i])
			++closedCount;
	}
	return closedCount;
}
