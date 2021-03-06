#pragma once
#include "ProbToWinWithCard.h"
#include "CardsCombos.h"
#include <algorithm>
#include <map>

struct tableEVandProbs {
	string NameCardOrCombo;
	vector<float> EVsEdges; //here I'll save EVs;
	vector<float> probToWinSd; //and for each of them I'll have prob (including P2,P3,P4)
	vector<float> antiEVforThisEdge;
};

class EqSummTables
{
public:
	EqSummTables(ProbToWinWithCard* PTWC, CardsCombos* cardsCombos);	
	
	vector<tableEVandProbs> vTableData;
	int indexForVectorAccess(string cardOrCombo);
private:
	void generateTableData();
	void tableDataToVTableData();
	void createEVsAndProbsTable(const vector<comboInfoFixedSituation>& situations, 
						const vector<int>& combosCount, const string& nameToSave);

	map<string, tableEVandProbs> tableData;
	ProbToWinWithCard* itsPTWC;
	CardsCombos* itsCardsCombos;
};

