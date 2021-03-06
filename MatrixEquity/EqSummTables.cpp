#include "EqSummTables.h"

EqSummTables::EqSummTables(ProbToWinWithCard* PTWC, CardsCombos* cardsCombos)
{
	itsPTWC = PTWC;
	itsCardsCombos = cardsCombos;
	generateTableData();
	tableDataToVTableData();
}

int EqSummTables::indexForVectorAccess(string cardOrCombo)
{
	if (cardOrCombo.length() == 1) {
		return itsCardsCombos->cardToIndex(cardOrCombo.c_str()[0]);
	}
	else {
		return itsCardsCombos->comboToIndex(cardOrCombo.c_str()) + (int) itsCardsCombos->cards.size();
	}
}

void EqSummTables::generateTableData()
{
	//generate table for cards;
	for (char card : itsCardsCombos->cards) {
		vector<comboInfoFixedSituation> situations;
		for (string combo : itsCardsCombos->combosWithCard[card]) {
			situations.insert(situations.end(), (itsPTWC->combosEvProbGlobal[combo]).begin(),
				(itsPTWC->combosEvProbGlobal[combo]).end());
		}
		sort(situations.rbegin(), situations.rend());
		vector<int> combosCount;
		for (comboInfoFixedSituation sit : situations) {
			if (sit.combo[0] == sit.combo[1]) {
				combosCount.push_back(3);
			}
			else if (sit.combo[2] =='o'){
				combosCount.push_back(3);
			}
			else if (sit.combo[2] == 's') {
				combosCount.push_back(1);
			}
			else {
				//got error;
				cout << "arror in data";
				int a;
				cin >> a;
			}
		}

		string strCard;
		strCard = card;
		createEVsAndProbsTable(situations, combosCount, strCard);
	}

	//generate table for combos
	for (string combo : itsCardsCombos->combos) {
		vector<comboInfoFixedSituation> situations = itsPTWC->combosEvProbGlobal[combo];
		sort(situations.rbegin(), situations.rend()); //sort from biggest down to smallest;
		vector<int> combosCount;
		combosCount.resize(15, 1);

		createEVsAndProbsTable(situations, combosCount, combo);
	}
}

void EqSummTables::tableDataToVTableData()
{
	//cards, then combos;
	vTableData.resize(itsCardsCombos->cards.size() + itsCardsCombos->combos.size());

	for (auto elem : tableData) {
		int posInVector = indexForVectorAccess(elem.first);
		if ((posInVector < 0) || (posInVector > vTableData.size()) || 
					(vTableData[posInVector].EVsEdges.size() != 0)) {
			cout << "error in table data to vTableData";
			int a;
			cin >> a;
		}
		vTableData[posInVector] = elem.second;
	}
}

void EqSummTables::createEVsAndProbsTable(const vector<comboInfoFixedSituation>& situations, 
									const vector<int>& combosCount, const string& nameToSave) {
	int totalCombosCount = 0;
	for (int countC : combosCount) {
		totalCombosCount += countC;
	}
	//думаю що комбосКаунт наразі неправильно враховується, треба подивитись цей момент.

	vector<float> EVsEdges; //here I'll save EVs;
	vector<float> probToWinSd; //and for each of them I'll have prob (including P3,P4)
	vector<float> antiEVforThisEdge;
	float lastAntiEV = 0;
	float lastProbToWinSd = 0;
	float summOfProbsForTest = 0;
	for (int i = 0; i < situations.size(); ++i) {
		const comboInfoFixedSituation& situat = situations[i];
		EVsEdges.push_back(situat.evOfAI);
		lastProbToWinSd += ((float) combosCount[i] / (float)totalCombosCount) *
							(1 / (float)itsPTWC->totalPlayers) * situat.preFreq * situat.probToWinIfAI;
		probToWinSd.push_back(lastProbToWinSd);
		if (situat.evOfAI < 0) {
			lastAntiEV += ((float)combosCount[i] / (float)totalCombosCount) *
				(1 / (float)itsPTWC->totalPlayers) * situat.preFreq * situat.evOfAI;
		}
		antiEVforThisEdge.push_back(lastAntiEV);
		summOfProbsForTest += ((float)combosCount[i] / (float)totalCombosCount) *
							(1 / (float)itsPTWC->totalPlayers) * situat.preFreq;
	}

	//also need to save all 3 this vectors for each combo;
	tableEVandProbs tableEnP;
	tableEnP.NameCardOrCombo = nameToSave;
	tableEnP.EVsEdges = EVsEdges;
	tableEnP.probToWinSd = probToWinSd;
	tableEnP.antiEVforThisEdge = antiEVforThisEdge;
	tableData[nameToSave] = tableEnP;
}




/*
	1) Генерація списків комбо для непопадань, для 1го та для 2х попадань.
	мають бути і самі комбо і їх кількість.

	2) formTableOfSummsForCombosArray
	для кожного із списків комбо генеруємо:
		робимо розширення структури comboInfoFixedSituation:
			добавляємо стрінг - саму комбо
			та інт (або ж одразу флоат) - кількість цих комб.
			сортування - спочатку по ЕВ від пуша.
			це все запихуємо в один вектор.
		для всіх комбо їх списки ситуацій (з кількістю кожної комбо звісно ж) зливаємо в 1 вектор
		сортуємо цей вектор. По суті маємо сортування по ЕВ.

		йдемо по вектору, і для кожного ЕВ знаходимо нову часткову суму та зберігаємо у новий вектор.
		результатом буде флоат границя і відповідні їй пара флоатів - частота закриття клітинки
		відносно всього набору комб в даній множині та антиЕВ.
		 

	варіанти реалізації:
	а) для пункту (1) ми для попадання по 1 клітинці не враховуємо блокери взагалі. 
	Це дасть можливість нарахувати все завчасно та зберегти.

	саме цю реалізацію я одразу і робитиму, а далі подивлюсь на швидкодію, чи можна 
	добавити блокери чи вже і так все буде занадто повільно.
		
*/
