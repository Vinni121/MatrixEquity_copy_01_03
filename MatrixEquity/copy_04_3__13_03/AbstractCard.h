#include <string>
#include <vector>
#include <map>
using namespace std;
#pragma once
class AbstractCard
{
public:
	AbstractCard();
	AbstractCard(string aCard);
	char getRepresentator(bool isDoubleAndSecondInCombo = 0);
	string getAbstractCard();
	bool operator==(const AbstractCard rhd);
private:
	string itsAbstractCard;
	char itsRepresentator;
	char itsRepresentator2InCombo;
};

