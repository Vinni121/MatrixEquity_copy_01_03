#pragma once
#include "ProbToWinWithCard.h"
#include "CardsCombos.h"
#include <algorithm>
#include <map>

struct tableEVandProbs {
	vector<float> EVsEdges; //here I'll save EVs;
	vector<float> probToWinSd; //and for each of them I'll have prob (including P3,P4)
	vector<float> antiEVforThisEdge;
};

class EqSummTables
{
public:
	EqSummTables(const ProbToWinWithCard& PTWC, const CardsCombos& cardsCombos);	
	map<string, tableEVandProbs> tableData;

private:
	void generateTableData();
	void createEVsAndProbsTable(const vector<comboInfoFixedSituation>& situations, 
						const vector<int>& combosCount, const string& nameToSave);

	ProbToWinWithCard itsPTWC;
	CardsCombos itsCardsCombos;
};

