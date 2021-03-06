#pragma once
#include "MatrixStatus.h"
#include "ProbToWinWithCard.h"
#include "CardsCombos.h"
#include<set>
#include <cmath>
#include <iostream>
//#include <fileapi.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>
namespace fs = std::filesystem;

using namespace std;

struct FLayer {
	static const int totalStatuses = 65536;
	int turns = 0;
	float EV[totalStatuses] = { 0 };
	float AntiEV[totalStatuses] = { 0 };
};


class FixedMatrixLayers
{
public:
	FixedMatrixLayers(vector<string> inpMatrixCards, const ProbToWinWithCard& PTWWC, const CardsCombos& CardCombos);

	void SaveLayerToHdd(int linesInOneFile, FLayer& layer);
	void LoadLayerFromHdd(FLayer& layer);

	//int matrixToIndex(Matrix M);
	//Matrix indexToMatrix(int MatrixIndex);

	void FillStartLayer(FLayer* outputFL);
	void FillNextLayer(FLayer* currLayer, FLayer* nextLayer);


private:
	vector<string> matrixCards;
	ProbToWinWithCard itsPTWWC;
	CardsCombos itsCardCombos;
	
	void EVAndAntiEVFromXHits(MatrixStatus& MS, FLayer* currLayer, int hitsAmount,
		float& EV, float& antiEV);
	void EVAndAntiEVNoHits(MatrixStatus& MS, FLayer* currLayer, float& EV, float& antiEV);

	void EvAndAntiEvFor(MatrixStatus& MS, set<int>& hittedCells, FLayer* currLayer, string& combo,
		float& EV, float& antiEV);

	set<set<int>> formSetsOfHittedCells(MatrixStatus& MS, int hitsAmount);
	set<string> combosForXHits(MatrixStatus& MS, set<int>& hittedCells); //only 1 and 2 supported at the moment;
	set<string> combosFor2Hits(char chCard, char chCard_2);

	float p1_probToHitXCells(MatrixStatus MS, int X);
	float p2_probComboFromComboList(string& combo, int hitsAmount); //only 1 and 2 supported at the moment;


	const int totalStatuses = 65536;

};

