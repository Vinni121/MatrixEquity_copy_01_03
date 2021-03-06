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
