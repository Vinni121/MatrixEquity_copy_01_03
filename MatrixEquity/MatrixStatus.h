#pragma once


class MatrixStatus
{
public:
	MatrixStatus();
	MatrixStatus(bool inputClosedCells[16]);
	MatrixStatus(int inputClosedCellsAsInt);
	void closeC(int cellIndex);
	bool isMatrixDone();

	bool closedCells[16] = { 0 };
	int closedCellsAsInt = 0;
	int numClosedCells();
	float payout(int turnsLeft);
private:
	bool lineIsDone(int cell1, int cell2, int cell3, int cell4);

	const float payoutIfDone = (float)15.64; // big blinds for bingo;
	const float rakeEconomyFor1Turn = (float)0.05;

};

