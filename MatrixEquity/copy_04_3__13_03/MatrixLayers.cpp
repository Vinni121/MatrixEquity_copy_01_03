#include "MatrixLayers.h"

MatrixLayers::MatrixLayers(vector<AbstractCard> inputAvailableCards, map<int, AbstractCard> inputFixedPositions,
	const ProbToWinWithCard& PTWWC, const CardsCombos& CardCombos) {
	availableCards = inputAvailableCards;
	fixedPositions = inputFixedPositions;
	itsPTWWC = PTWWC;
	itsCardCombos = CardCombos;
}

void MatrixLayers::SaveLayerToHdd(int linesInOneFile, Layer& layer)
{
	//create directory for layer;
	string avDecription = "cards_";
	for (AbstractCard& ac : availableCards) {
		avDecription += ac.getAbstractCard() + "_";
	}
	string fixedDescription = "f_";
	for (auto elem : fixedPositions) {
		fixedDescription += to_string(elem.first) + "-" + elem.second.getAbstractCard() + "_";
	}
	string folder = "M_base\\" + avDecription + "\\" + fixedDescription;
	folder += "\\" + to_string(layer.turns);
	fs::create_directories(folder);

	int fileIndex = 0;
	int lines = 0;
	int firstElemInFile = 0;
	string dataToSave;
	string NavigationString;
	ofstream fData;
	for (auto elem : layer.mapToEVandAntiEV) {
		dataToSave += to_string(elem.first) + "\t";
		dataToSave += to_string(elem.second.first) + "\t" + to_string(elem.second.second) + "\n";
		if (lines == 0) {
			firstElemInFile = elem.first;
			NavigationString += to_string(firstElemInFile) + "\n";
		}
		++lines;
		if (lines % linesInOneFile == 0) {
			fData.open(folder + "\\" + "data_" + to_string(fileIndex) + ".txt");
			fData << dataToSave;
			fData.close();
			++fileIndex;
			lines = 0;
		}
	}
	if (lines != 0) {
		fData.open(folder + "\\" + "data_" + to_string(fileIndex) + ".txt");
		fData << dataToSave;
		fData.close();
	}

	fData.open(folder + "\\" + "_navigation.txt");
	fData << NavigationString;
	fData.close();
}


Layer MatrixLayers::StartLayer() {
	Layer startLayer;
	startLayer.turns = 0;
	for (int MatrixIndex = 0; MatrixIndex < pow(availableCards.size(), 16); ++MatrixIndex) {
		//add skip or change loop to support fixed cards;

		if (MatrixIndex % 10000 == 0) {
			cout << MatrixIndex/10000 << endl;
		}	
		float EV = 0;
		/*Matrix M = indexToMatrix(MatrixIndex);  //for test speed;
		if (M.isMatrixDone()) {
			EV = M.payout(startLayer.turns);
		}*/
		float antiEV = 0;
		startLayer.mapToEVandAntiEV[MatrixIndex] = make_pair(EV, antiEV);
	}

	return startLayer;
}

Layer MatrixLayers::NextLayer(Layer currLayer) {
	Layer newLayer;
	newLayer.turns = currLayer.turns + 1;
	for (int MatrixIndex = 0; MatrixIndex < pow(availableCards.size(), 16); ++MatrixIndex) {
		//add skip or change loop to support fixed cards;

		AbstractCard cards[16];/////for speed test
		Matrix M(cards); /////for speed test instead of next line;
		///////Matrix M = indexToMatrix(MatrixIndex);
		float EV = 0;
		float antiEV = 0;
		if (M.isMatrixDone()) {
			EV = M.payout(newLayer.turns);
			antiEV = 0;
		}
		else {
			pair<float, float> partEvsFrom0Hits = EVAndAntiEVFromXHits(M, currLayer, 0);
			pair<float, float> partEvsFrom1Hit = EVAndAntiEVFromXHits(M, currLayer, 1);
			pair<float, float> partEvsFrom2Hits = EVAndAntiEVFromXHits(M, currLayer, 2);

			EV = partEvsFrom0Hits.first + partEvsFrom1Hit.first + partEvsFrom2Hits.first;
			antiEV = partEvsFrom0Hits.second + partEvsFrom1Hit.second + partEvsFrom2Hits.second;
		}
		newLayer.mapToEVandAntiEV[MatrixIndex] = make_pair(EV, antiEV);
		if (MatrixIndex % 1000 == 0) 
			cout << "matrix Index " << MatrixIndex << endl;
	}
	return newLayer;
}



pair<float, float> MatrixLayers::EVAndAntiEVFromXHits(Matrix& M, Layer& currLayer, int hitsAmount) {

	float collectedEV = 0;
	float collectedAntiEV = 0;

	//for test speed of operation;
	//what if I'll get this AVs very-very fast? 
	//lets test variant where I have it immediately (but that's not true)
	//return make_pair(collectedEV, collectedAntiEV);

	//no hits to matrix;
	if (hitsAmount == 0) {
		return EVAndAntiEVNoHits(M, currLayer);
	}
	//for all variant of hitsAmount not "Done" cells in matrix
	set<set<int>> setsOfHittedCells = formSetsOfHittedCells(M, hitsAmount);
	for (set<int> hittedCells: setsOfHittedCells) {		
		float collEvForThisCell = 0;
		float collAntiEvForThisCell = 0;

		// for all combo that are ok for this cell
		set<string> combosWithCardFromCell = combosForXHits(M, hittedCells);
			
		for (string combo : combosWithCardFromCell) {
			//at this moment we know that we hitted cells, so we've hitted to this combolist;
			pair<float, float> EVsForCombo = EvAndAntiEvFor(M, hittedCells,
				currLayer, combo);

			//here is (P2) - // prob to have right this combo among all combos in combolist;
			float P2 = p2_probComboFromComboList(combo, hitsAmount);
			collEvForThisCell += P2 * EVsForCombo.first;
			collAntiEvForThisCell += P2 * EVsForCombo.second;
		}
		//get probability to have right 2 fixed cells in hero hand (P1);
		float prob_2FixedCell = p1_probToHitXCells(M, hitsAmount);

		collectedEV += prob_2FixedCell * collEvForThisCell;
		collectedAntiEV += prob_2FixedCell * collAntiEvForThisCell;
	}
	return make_pair(collectedEV, collectedAntiEV);
}

pair<float, float> MatrixLayers::EVAndAntiEVNoHits(Matrix& M, Layer& currLayer) {
	pair<float, float> EVs = currLayer.mapToEVandAntiEV[matrixToIndex(M)];
	float prob_noHits = p1_probToHitXCells(M, 0);
	return make_pair(prob_noHits * EVs.first, prob_noHits * EVs.second);
}

set<set<int>> MatrixLayers::formSetsOfHittedCells(Matrix & M, int hitsAmount) {
	set<set<int>> setOfHittedCells;
	for (int ind = 0; ind < pow(16, hitsAmount); ++ind) {
		set<int> cellsSet;
		for (int i = 0; i < hitsAmount; ++i) {
			int indTail = ind;
			cellsSet.insert(indTail % 16);
			indTail /= 16;
		}
		if (cellsSet.size() != hitsAmount) //don't need to check cells set smaller that hitsAmount;
			continue;

		bool allHittedCellsNotDone = true;
		for (int hittedCell : cellsSet) {
			if (M.itsCards[hittedCell].getAbstractCard() == "D") {
				allHittedCellsNotDone = false;
			}
		}
		if (allHittedCellsNotDone) {
			setOfHittedCells.insert(cellsSet);
		}	
	}
	return setOfHittedCells;
}

set<string> MatrixLayers::combosForXHits(Matrix& M, set<int>& hittedCells) {
	vector<int> hitted(hittedCells.begin(), hittedCells.end());
	if (hitted.size() == 2) {
		char chCard = M.itsCards[hitted[0]].getRepresentator();
		char chCard_2 = M.itsCards[hitted[1]].getRepresentator(true);
		set<string> combosWithCardFromCell = combosFor2Hits(chCard, chCard_2);
		return combosWithCardFromCell;
	}
	if (hitted.size() == 1) {
		char chCard = M.itsCards[hitted[0]].getRepresentator();
		set<string> combosWithCardFromCell = itsCardCombos.combosWithCard[chCard];
		return combosWithCardFromCell;
	}
	cout << "error in p1 calculating";
	int a;
	cin >> a;
	set<string> emptySet;
	return emptySet;
}

set<string> MatrixLayers::combosFor2Hits(char chCard, char chCard_2) {
	set<string> combosWithCardFromCell;
	if (chCard == chCard_2) {
		string comboPair;
		comboPair = chCard + chCard_2;
		combosWithCardFromCell = { comboPair };
	}
	else {
		int posCard1 = 0, posCard2 = 0;
		for (int cardInd = 0; cardInd < itsCardCombos.cards.size(); ++cardInd) {
			if (itsCardCombos.cards[cardInd] == chCard)
				posCard1 = cardInd;
			if (itsCardCombos.cards[cardInd] == chCard_2)
				posCard2 = cardInd;
		}
		string comboFirst2Chars;
		if (posCard1 > posCard2)
			comboFirst2Chars = posCard1 + posCard2;
		else
			comboFirst2Chars = posCard2 + posCard1;
		string comboS, comboO;
		comboS = comboFirst2Chars + 's';
		comboO = comboFirst2Chars + 'o';
		combosWithCardFromCell = { comboS, comboO };
	}
	return combosWithCardFromCell;
}

float MatrixLayers::p1_probToHitXCells(Matrix M, int X) {
	int notDoneCellsInMatrix = 0;
	for (int cell = 0; cell < 16; ++cell) {
		if (M.itsCards[cell].getAbstractCard() != "D") {
			++notDoneCellsInMatrix;
		}
	}
	int combosForOneCell = 51 - (notDoneCellsInMatrix - 1);
	int totalCombos = 52 * 51 / 2;
	float prob_1FixedCell = (float)combosForOneCell / (float)totalCombos;

	int combosForTwoCells = 1;
	float prob_2FixedCell = (float)combosForTwoCells / (float)totalCombos;

	if (X == 1)
		return prob_1FixedCell;
	if (X == 2)
		return prob_2FixedCell;
	if (X == 0)
		return (1 - prob_1FixedCell - prob_2FixedCell); //change it for Omaha;!!!

	cout << "error in p1 calculating";
	int a;
	cin >> a;
	return -9999999;
}

float MatrixLayers::p2_probComboFromComboList(string& combo, int hitsAmount) {
	//more correct algorhitm - get combo and combolist as input 
	//find combos count for combolist and count combos for input combo;

	if (hitsAmount == 1) {
		float P2 = 0;
		if (combo[0] == combo[1]) {
			P2 = (float)3.0 / (float)51.0;
		}
		else if (combo[2] == 'o') {
			P2 = (float)3.0 / (float)51.0;
		}
		else if (combo[2] == 's') {
			P2 = (float)1.0 / (float)51.0;
		}
		else {
			cout << "not correct combo when looking for its probability in set, 1";
			char c;
			cin >> c;
		}
		return P2;
	}
	if (hitsAmount == 2) {
		float P2 = 0;
		if (combo[0] == combo[1]) {
			P2 = (float)1.0;
		}
		else if (combo[2] == 'o') {
			P2 = (float)3.0 / (float)4.0;
		}
		else if (combo[2] == 's') {
			P2 = (float)1.0 / (float)4.0;
		}
		else {
			cout << "not correct combo when looking for its probability in set, 2";
			char c;
			cin >> c;
		}
		return P2;
	}

	cout << "error in p2 calculating";
	int a;
	cin >> a;
	return -9999999;
}

pair<float, float> MatrixLayers::EvAndAntiEvFor(Matrix& M, set<int>& hittedCells,
	Layer& currLayer, string& combo) {
	float EvFor = 0;
	float AntiEvFor = 0;
	//for all situations that can be for this combo
	vector<comboInfoFixedSituation> situations = itsPTWWC.combosEvProbGlobal[combo];
	for (comboInfoFixedSituation& situat : situations) {
		// calculate P = P3*P4 - probabilities to have all this conditions;
		// prob to have right this hero position
		//and prob to have right this situation for this position (prefrequency)
		float P = (float)1.0 / (float)itsPTWWC.totalPlayers * situat.preFreq;

		//calculate EV of matrix if hero fold;
		pair<float, float> MatrixEvsHeroFold = currLayer.mapToEVandAntiEV[matrixToIndex(M)];
		//calculate EV of matrix if hero push;
		Matrix MatrixIfWin = M; //can I use standart copy constructor??;
		for (int hittedCell : hittedCells) {
			MatrixIfWin.cellIsDone(hittedCell);
		}
		float MainEvHeroPush =
			currLayer.mapToEVandAntiEV[matrixToIndex(MatrixIfWin)].first * situat.probToWinIfAI +
			currLayer.mapToEVandAntiEV[matrixToIndex(M)].first * (1 - situat.probToWinIfAI);
		float AntiEvHeroPush = 
			currLayer.mapToEVandAntiEV[matrixToIndex(MatrixIfWin)].second * situat.probToWinIfAI +
			currLayer.mapToEVandAntiEV[matrixToIndex(M)].second * (1 - situat.probToWinIfAI);

		//if EVpush - EVfold > -EVDiff for this situation
		if (MainEvHeroPush - MatrixEvsHeroFold.first > -situat.evOfAI) {
			//then hero plays all-in, so 
			if (situat.evOfAI > 0) {
				EvFor += P * MainEvHeroPush;
				AntiEvFor += P * AntiEvHeroPush;
			}
			else {
				//increase by P * (EVpush - (-EVDiff)), and antiEV increase by P*EVDiff.
				EvFor += P * MainEvHeroPush - (-situat.evOfAI);
				AntiEvFor += P * (AntiEvHeroPush + situat.evOfAI);
			}					
		}
		else {
			//else hero folds, so 
			//increase matrix EV by P * EVFold
			EvFor += P * MatrixEvsHeroFold.first;
			AntiEvFor += P * MatrixEvsHeroFold.second;
			
		}
	}
	return make_pair(EvFor, AntiEvFor);
}

int MatrixLayers::findPosInAvailCards(AbstractCard aCard) {
	for (int pos = 0; pos < availableCards.size(); ++pos) {
		if (aCard == availableCards[pos]) {
			return pos;
			break;
		}
	}
	cout << "critical error in matrix to index function" << endl;
	int stop;
	cin >> stop;	
	return -1;
}

int MatrixLayers::matrixToIndex(Matrix M) {
	int resultIndex = 0;
	for (int i = 0; i < 16; ++i) {
		int inAvailIndex = findPosInAvailCards(M.itsCards[i]);
		resultIndex = resultIndex * (int) availableCards.size() + inAvailIndex;
	}
	return resultIndex;
}

Matrix MatrixLayers::indexToMatrix(int index)
{
	int miHead = index;
	AbstractCard card[16] = { AbstractCard("_") };
	for (int cardInd = 15; cardInd >= 0; cardInd--) {
		if (fixedPositions.count(cardInd) != 0) {
			card[cardInd] = fixedPositions[cardInd];
		}
		else {
			int miTail = miHead % availableCards.size();
			miHead = miHead / (int) availableCards.size();
			card[cardInd] = availableCards[miTail];
		}
	}
	Matrix M(card);
	return M;
}
