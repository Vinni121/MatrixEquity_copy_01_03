#pragma once
#include <vector>
#include<set>
using namespace std;
#include "MatrixStatus.h"
#include <iostream>

class setsOfHittedCells
{
public:
	static const int totalStatuses = 65536; //1000; //  //for each matrix for 0 (empty?), 1, 2 hitted cells;
	setsOfHittedCells();
	void formForAllMatrixStatuses();

	vector<MatrixStatus> vChildMatrixesForXHits[totalStatuses][3];
	float probToHit[totalStatuses][3];
private:
	//vector<vector<int>> setsOfChildren[totalStatuses][3];
	void formSetsOfHittedCells(int MatrixIndex, int hitsAmount);
	float p1_probToHitXCells(MatrixStatus& MS, int X);
};

