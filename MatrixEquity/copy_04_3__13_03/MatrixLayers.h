#pragma once
#include "Matrix.h"
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

struct Layer {
	int turns = 0;
	map<int, pair<float, float>> mapToEVandAntiEV;
};

class MatrixLayers
{
public:
	MatrixLayers(vector<AbstractCard> inputAvailableCards, map<int, AbstractCard> inputFixedPositions, 
		const ProbToWinWithCard& PTWWC, const CardsCombos& CardCombos);
	//fixed positions not support yet;

	void SaveLayerToHdd(int linesInOneFile, Layer& layer);
	void LoadLayerFromHdd(Layer& layer);

	int matrixToIndex(Matrix M);
	Matrix indexToMatrix(int MatrixIndex);

	Layer StartLayer();
	Layer NextLayer(Layer currLayer);


private:
	vector<AbstractCard> availableCards;
	map<int, AbstractCard> fixedPositions;
	ProbToWinWithCard itsPTWWC; 
	CardsCombos itsCardCombos;

	bool isInRAM; //it's used on calculation stage + before read/write by parts released;

	int findPosInAvailCards(AbstractCard aCard);

	//NextLayer subfunctions:
	pair<float, float> EVAndAntiEVNoHits(Matrix& M, Layer& currLayer);
	pair<float, float> EVAndAntiEVFromXHits(Matrix& M, Layer& currLayer, int hits);
	pair<float, float> EvAndAntiEvFor(Matrix& M, set<int>& hittedCells,
		Layer& currLayer, string& combo);
	set<set<int>> formSetsOfHittedCells(Matrix & M, int hitsAmount);
	set<string> combosForXHits(Matrix& M, set<int>& hittedCells); //only 1 and 2 supported at the moment;
	set<string> combosFor2Hits(char chCard, char chCard_2);
	float p1_probToHitXCells(Matrix M, int X);
	float p2_probComboFromComboList(string& combo, int hitsAmount); //only 1 and 2 supported at the moment;



};

