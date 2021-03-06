#include "FixedMatrixLayers.h"

FixedMatrixLayers::FixedMatrixLayers(vector<string> inpMatrixCards, const ProbToWinWithCard& PTWWC, const CardsCombos& CardCombos) {
	matrixCards = inpMatrixCards;
	itsPTWWC = PTWWC;
	itsCardCombos = CardCombos;
}



void FixedMatrixLayers::FillStartLayer(FLayer* startLayer) {
	startLayer->turns = 0;
	for (int MatrixIndex = 0; MatrixIndex < totalStatuses; ++MatrixIndex) {

		if (MatrixIndex % 10000 == 0) {
			cout << "new variant" << MatrixIndex / 10000 << endl;
		}
		MatrixStatus MS(MatrixIndex);
		if (MS.isMatrixDone()) {
			startLayer->EV[MatrixIndex] = MS.payout(startLayer->turns);
		}
		else {
			startLayer->EV[MatrixIndex] = 0;
		}
		startLayer->AntiEV[MatrixIndex] = 0;
	}
}


void FixedMatrixLayers::FillNextLayer(FLayer* currLayer, FLayer* nextLayer) {

	nextLayer->turns = currLayer->turns + 1;
	for (int MatrixIndex = 0; MatrixIndex < totalStatuses; ++MatrixIndex) {

		MatrixStatus MS(MatrixIndex);
		float EV = 0;
		float antiEV = 0;
		if (MS.isMatrixDone()) {
			EV = MS.payout(nextLayer->turns);
			antiEV = 0;
		}
		else {
			float EV_0 = 0.0, antiEV_0 = 0.0;
			EVAndAntiEVFromXHits(MS, currLayer, 0, EV_0, antiEV_0);
			
			float EV_1 = 0.0, antiEV_1 = 0.0;
			EVAndAntiEVFromXHits(MS, currLayer, 1, EV_1, antiEV_1);

			float EV_2 = 0.0, antiEV_2 = 0.0;
			EVAndAntiEVFromXHits(MS, currLayer, 2, EV_2, antiEV_2);

			EV = EV_0 + EV_1 + EV_2;
			antiEV = antiEV_0 + antiEV_1 + antiEV_2;


			//інший підхід (реалізація потім, спочатку дебаг того, що є).
			/*знаходимо можливі набори клітинок, які можуть бути шотнуті.
			Також знаходимо імовірність такого сценарію.
			тобто ми знаємо що Х клітинок живі--->шанс попасти в 1 клітинку, в 2, в 3.
			мабуть при попаданні в 1 клітинку я не рахуватиму блокери (карти в інших живих клітинках?)
			Чи рахуватиму?
			якщо без враховування блокерів, то
			знаємо шанс, знаємо клітивки які шотнули.
			Пускаємо ф-ію, яка поверне ЕВ від таких ситуацій.
			
				дана ф-ія бере різницю ЕВ м-ць наступного слою (шот, не шот).
				і залежно від цього значення та від карт/карти які шотнулись вона повинна побачити
				імовірність того, що ми закриваємо клітинку.

				
			
			*/
		}
		nextLayer->EV[MatrixIndex] = EV;
		nextLayer->AntiEV[MatrixIndex] = antiEV;
		if (MatrixIndex % 10 == 0)
			cout << "matrix Index " << MatrixIndex << endl;
	}
}


void FixedMatrixLayers::EVAndAntiEVFromXHits(MatrixStatus& MS, FLayer* currLayer, int hitsAmount,
											float& EV, float& antiEV) {
	float collectedEV = 0;
	float collectedAntiEV = 0;

	//no hits to matrix;
	if (hitsAmount == 0) {
		EVAndAntiEVNoHits(MS, currLayer, EV, antiEV);
	}
	else {
		//for all variant of hitsAmount not "Done" cells in matrix
		set<set<int>> setsOfHittedCells = formSetsOfHittedCells(MS, hitsAmount);
		for (set<int> hittedCells : setsOfHittedCells) {
			float collEvForThisCell = 0;
			float collAntiEvForThisCell = 0;

			// for all combo that are ok for this cell
			set<string> combosWithCardFromCell = combosForXHits(MS, hittedCells);

			for (string combo : combosWithCardFromCell) {
				//at this moment we know that we hitted cells, so we've hitted to this combolist;
				float EVForCombo = 0, AntiEVForCombo = 0;
				EvAndAntiEvFor(MS, hittedCells, currLayer, combo, EVForCombo, AntiEVForCombo);

				//here is (P2) - // prob to have right this combo among all combos in combolist;
				float P2 = p2_probComboFromComboList(combo, hitsAmount);
				collEvForThisCell += P2 * EVForCombo;
				collAntiEvForThisCell += P2 * AntiEVForCombo;
			}
			//get probability to have right 2 fixed cells in hero hand (P1);
			float prob_2FixedCell = p1_probToHitXCells(MS, hitsAmount);

			collectedEV += prob_2FixedCell * collEvForThisCell;
			collectedAntiEV += prob_2FixedCell * collAntiEvForThisCell;
		}
		EV = collectedEV;
		antiEV = collectedAntiEV;
	}
}

void FixedMatrixLayers::EVAndAntiEVNoHits(MatrixStatus& MS, FLayer* currLayer, float& EV, float& antiEV) {
	EV = currLayer->EV[MS.closedCellsAsInt];
	antiEV = currLayer->AntiEV[MS.closedCellsAsInt];
	float prob_noHits = p1_probToHitXCells(MS, 0);
}
float FixedMatrixLayers::p1_probToHitXCells(MatrixStatus MS, int X) {
	int notDoneCellsInMatrix = 16 - MS.numClosedCells();
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

set<set<int>> FixedMatrixLayers::formSetsOfHittedCells(MatrixStatus& MS, int hitsAmount) {
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
			if (MS.closedCells[hittedCell]) {
				allHittedCellsNotDone = false;
			}
		}
		if (allHittedCellsNotDone) {
			setOfHittedCells.insert(cellsSet);
		}
	}
	return setOfHittedCells;
}

float FixedMatrixLayers::p2_probComboFromComboList(string& combo, int hitsAmount) {
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

void FixedMatrixLayers::EvAndAntiEvFor(MatrixStatus& MS, set<int>& hittedCells, FLayer* currLayer, string& combo,
	float& EV, float& antiEV) {
	float EvFor = 0;
	float AntiEvFor = 0;
	//for all situations that can be for this combo
	//vector<comboInfoFixedSituation>& situations = itsPTWWC.combosEvProbGlobal[combo];
	for (comboInfoFixedSituation& situat : itsPTWWC.combosEvProbGlobal[combo]) {
		// calculate P = P3*P4 - probabilities to have all this conditions;
		// prob to have right this hero position
		//and prob to have right this situation for this position (prefrequency)
		float P = (float)1.0 / (float)itsPTWWC.totalPlayers * situat.preFreq;

		//calculate EV of matrix if hero fold;
		float EvHeroFold = 0, antiEVHeroFold = 0;
		EvHeroFold = currLayer->EV[MS.closedCellsAsInt];
		antiEVHeroFold = currLayer->AntiEV[MS.closedCellsAsInt];
		//calculate EV of matrix if hero push;
		MatrixStatus MatrixIfWin = MS; //can I use standart copy constructor??;
		for (int hittedCell : hittedCells) {
			MatrixIfWin.closeC(hittedCell);
		}
		float MainEvHeroPush =
			currLayer->EV[MatrixIfWin.closedCellsAsInt] * situat.probToWinIfAI +
			currLayer->EV[MS.closedCellsAsInt] * (1 - situat.probToWinIfAI);
		float AntiEvHeroPush =
			currLayer->AntiEV[MatrixIfWin.closedCellsAsInt] * situat.probToWinIfAI +
			currLayer->AntiEV[MS.closedCellsAsInt] * (1 - situat.probToWinIfAI);

		//if EVpush - EVfold > -EVDiff for this situation
		if (MainEvHeroPush - EvHeroFold > -situat.evOfAI) {
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
			EvFor += P * EvHeroFold;
			AntiEvFor += P * antiEVHeroFold;

		}
	}
	EV = EvFor;
	antiEV = AntiEvFor;
}


set<string> FixedMatrixLayers::combosForXHits(MatrixStatus& MS, set<int>& hittedCells) {
	vector<int> hitted(hittedCells.begin(), hittedCells.end());
	if (hitted.size() == 2) {
		string chFullCard_1 = matrixCards[hitted[0]];
		char rank_1 = chFullCard_1[0];
		string chFullCard_2 = matrixCards[hitted[1]];
		char rank_2 = chFullCard_2[0];
		set<string> combosWithCardFromCell = combosFor2Hits(rank_1, rank_2);
		return combosWithCardFromCell;
	}
	if (hitted.size() == 1) {
		string chFullCard_1 = matrixCards[hitted[0]];
		char rank_1 = chFullCard_1[0];
		set<string> combosWithCardFromCell = itsCardCombos.combosWithCard[rank_1];
		return combosWithCardFromCell;
	}
	cout << "error in p1 calculating";
	int a;
	cin >> a;
	set<string> emptySet;
	return emptySet;
}

set<string> FixedMatrixLayers::combosFor2Hits(char chCard, char chCard_2) {
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