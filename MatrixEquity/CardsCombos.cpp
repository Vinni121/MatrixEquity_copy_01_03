#include "CardsCombos.h"


CardsCombos::CardsCombos(vector<string> combosList) {
	cards = { '2','3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A' };
	initCombos(combosList);
	initcombosWithCard();

}

void CardsCombos::initCombos(vector<string> combosList) {
	for (auto elem : combosList) {
		combos.push_back(elem);
	}
}

void CardsCombos::initcombosWithCard() {
	for (char card : cards) {
		combosWithCard[card]; //create empty vector;
	}
	for (string combo : combos) {
		combosWithCard[combo[0]].insert(combo);
		combosWithCard[combo[1]].insert(combo);
	}
}

string CardsCombos::indexToCombo(int index) {
	int i = index / 13;
	int j = index % 13;
	char LocalHand[4];
	if (i == j) //pair
	{
		LocalHand[0] = cards[0];
		LocalHand[1] = cards[1];
		LocalHand[2] = '\0';
	}
	if (i > j)
	{
		LocalHand[0] = cards[0];
		LocalHand[1] = cards[1];
		LocalHand[2] = 's';
		LocalHand[3] = '\0'; //sumbol "end of string";
	}
	if (i < j)
	{
		LocalHand[0] = cards[1];
		LocalHand[1] = cards[0];
		LocalHand[2] = 'o';
		LocalHand[3] = '\0'; //sumbol "end of string";
	}
	string sResult(LocalHand);
	return sResult;
}

int CardsCombos::cardToIndex(const char card) {
	int cardsArraySize = (int) cards.size();
	for (int cardNum = 0; cardNum < cardsArraySize; ++cardNum) {
		if (card == cards[cardNum]) {
			return cardNum;
		}
	}
	return -1; //error, just to get error meassge and avoid warning;
}
int CardsCombos::comboToIndex(const char* comboChar) {
	int cardIndex[2] = { 0,0 };
	for (int i = 0; i < 2; ++i) {
		cardIndex[i] = cardToIndex(comboChar[i]);
	}
	if (comboChar[2] != 'o') {
		return cardIndex[0] * 13 + cardIndex[1];
	}
	else {
		return cardIndex[1] * 13 + cardIndex[0];
	}
}

/*
void CardsCombos::FunctionMakeSetOfHands()
{
	char HandElement[2], LocalHand[5];
	for (int i = 0; i < 13; i++)
	{
		HandElement[0] = cards[i];
		for (int j = 0; j < 13; j++)
		{
			HandElement[1] = cards[j];			
			


			// порядковий номер руки (правда ніде не збережений) = (i-2)*13 + (j-1)
			int HandNumber = (i - 2) * 13 + (j - 2);
			for (int k = 0; k <= 4; k++) SetOfHands[HandNumber][k] = LocalHand[k];
			//cout << "\n" << HandNumber<<"  hand = "<<LocalHand;
		}
	}
}

*/