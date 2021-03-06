#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <exception>
#include <map>
#include <vector>
#include <set>
using namespace std;
class CardsCombos
{
public:
	CardsCombos() {};
	CardsCombos(vector<string> combosList);

	int cardToIndex(const char card);
	int comboToIndex(const char* comboChar);
	string indexToCombo(int index);

	vector<char> cards;
	vector<string> combos;
	map<char, set<string>> combosWithCard;
private:
	//void FunctionMakeSetOfHands();
	void initCombos(vector<string> combosList);
	void initcombosWithCard();
};

