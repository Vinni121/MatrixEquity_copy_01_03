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
	vector<char> cards;
	vector<string> combos;
	map<char, set<string>> combosWithCard;
private:
	void initCombos(vector<string> combosList);
	void initcombosWithCard();
};

