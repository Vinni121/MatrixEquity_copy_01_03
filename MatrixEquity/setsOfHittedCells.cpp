#include "setsOfHittedCells.h"

setsOfHittedCells::setsOfHittedCells() {
}

void setsOfHittedCells::formForAllMatrixStatuses() {
	for (int indexMS = 0; indexMS < totalStatuses; ++indexMS) {
		for (int hits = 1; hits <= 2; ++hits) {
			formSetsOfHittedCells(indexMS, hits);
		}
		if (indexMS % 1000 == 0) {
			cout <<"children list formed for" << indexMS << endl;
		}
	}
}


void setsOfHittedCells::formSetsOfHittedCells(int MatrixIndex, int hitsAmount) {

	set<vector<int>> setOfHittedCells;
	set<MatrixStatus> setMS; //all child matrixes for current if hitted by hitsAmount;
	float probToGetXHits;

	MatrixStatus MS(MatrixIndex);
	for (int ind = 0; ind < pow(16, hitsAmount); ++ind) {
		set<int> cellsSet;
		int indTail = ind;
		for (int i = 0; i < hitsAmount; ++i) {			
			cellsSet.insert(indTail % 16);
			indTail /= 16;
		}
		if (cellsSet.size() != hitsAmount) //don't need to check cells set smaller that hitsAmount;
			continue;

		bool allHittedCellsNotDone = true;
		for (int hittedCell : cellsSet) {
			if (MS.closedCells[hittedCell]) {
				allHittedCellsNotDone = false;
			}
		}
		if (allHittedCellsNotDone) {
			vector<int> vCellsSet(cellsSet.begin(), cellsSet.end());
			setOfHittedCells.insert(vCellsSet);

			MatrixStatus ChildMatrix = MS;
			for (int cellInd : vCellsSet) {
				ChildMatrix.closeC(cellInd);
			}
			setMS.insert(ChildMatrix);
		}
	}
	//vector<vector<int>> vResultHittedCells(setOfHittedCells.begin(), setOfHittedCells.end());
	//setsOfChildren[MatrixIndex][hitsAmount] = vResultHittedCells;

	vector<MatrixStatus> vResultChildMatrixes(setMS.begin(), setMS.end());
	vChildMatrixesForXHits[MatrixIndex][hitsAmount] = vResultChildMatrixes;
	probToGetXHits = p1_probToHitXCells(MS, hitsAmount);
	probToHit[MatrixIndex][hitsAmount] = probToGetXHits;
}


float setsOfHittedCells::p1_probToHitXCells(MatrixStatus& MS, int X) {
	int notDoneCellsInMatrix = 16 - MS.numClosedCells();
	int combosForOneCell = 51 - (notDoneCellsInMatrix - 1);
	int totalCombos = 52 * 51 / 2;
	float prob_1FixedCell = (float)combosForOneCell / (float)totalCombos;

	int combosForTwoCells = 1;
	float prob_2FixedCell = (float)combosForTwoCells / (float)totalCombos;

	int combos0cells = (52 - notDoneCellsInMatrix) * (51 - notDoneCellsInMatrix) / 2;
	float prob_0FixedCell = (float)combos0cells / (float)totalCombos;

	if (X == 1)
		return prob_1FixedCell;
	if (X == 2)
		return prob_2FixedCell;
	if (X == 0)
		return prob_0FixedCell;

	cout << "error in p1 calculating";
	int a;
	cin >> a;
	return -9999999;
}