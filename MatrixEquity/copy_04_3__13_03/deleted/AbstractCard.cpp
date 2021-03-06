#include "AbstractCard.h"


AbstractCard::AbstractCard() {
	itsAbstractCard = "";
	itsRepresentator = '\0';
	itsRepresentator2InCombo = '\0';
}
AbstractCard::AbstractCard(string aCard) {
	itsAbstractCard = aCard;
	//default - it is a real card or "D" - done, cell is closed;
	itsRepresentator = aCard[0];
	itsRepresentator2InCombo = aCard[0];
	if (aCard == "L") {
		itsRepresentator = '7';
		itsRepresentator2InCombo = '5';
	}
	if (aCard == "H") {
		itsRepresentator = 'J';
		itsRepresentator2InCombo = 'Q';
	}
	if (aCard == "R") {
		itsRepresentator = '9';
		itsRepresentator2InCombo = 'T';
	}
}
char AbstractCard::getRepresentator(bool isDoubleAndSecondInCombo) {
	if (!isDoubleAndSecondInCombo)
		return itsRepresentator;
	else
		return itsRepresentator2InCombo;
}
string AbstractCard::getAbstractCard() {
	return itsAbstractCard;
}

bool AbstractCard::operator==(const AbstractCard rhd) {
	if (itsAbstractCard == rhd.itsAbstractCard)
		return true;
	else
		return false;
}