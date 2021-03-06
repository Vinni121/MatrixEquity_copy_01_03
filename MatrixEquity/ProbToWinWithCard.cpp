#include "ProbToWinWithCard.h"

int const comboInfoFixedSituation::operator<(const comboInfoFixedSituation& rhd) {
	if (evOfAI != rhd.evOfAI) 
		return (evOfAI < rhd.evOfAI);
	if (preFreq != rhd.preFreq)
		return (preFreq < rhd.preFreq);
	if (probToWinIfAI != rhd.probToWinIfAI)
		return (probToWinIfAI < rhd.probToWinIfAI);
	if (combo != rhd.combo)
		return (combo < rhd.combo);
	if (situationDescription != rhd.situationDescription)
		return (situationDescription < rhd.situationDescription);
	if (situationDescription_01 != rhd.situationDescription_01)
		return (situationDescription_01 < rhd.situationDescription_01);
	return 0;
}


ProbToWinWithCard::ProbToWinWithCard() {
	//std::vector<std::string> allFinalSituations = { "0000", "0001", "0010", "0011","0100", "0101", "0110", "0111","1000", "1001", "1010", "1011","1100", "1101", "1110", "1111"}
	totalPlayers = 4;
	positionName = { "BB", "SB", "BU", "CO" };
	vintFinalSituatM1AI = { "_0011","_0101", "_0110", "_0111","_1001", "_1010", "_1011","_1100", "_1101", "_1110", "_1111" };


	parseAllPstove();
	calc1_probToWinWithComboPostHeroAI();
	readTablesWithEVs();
	fillcombosEvProbGlobal();
}

std::map<std::string, float> ProbToWinWithCard::parsePstoveOneFile(std::string fileName, int numberOfPlayerToRead)
{
	std::map<std::string, float> resultMap;
	std::ifstream fin(fileName);
	if (!fin.is_open()) {
		std::cout << "file" << fileName << "open error" << std::endl;
		throw std::runtime_error("file" + fileName + "open error");
	}
	else {
		std::string token;
		std::string HandsAndProbs("");
		while (!fin.eof()) {
			fin >> token;
			if (token == "Hand") {
				int iPlayer;
				fin >> iPlayer;
				if (iPlayer == numberOfPlayerToRead) {
					fin >> token; //here is ":"
					fin >> token; //here is win equity
					float probWin;
					fin >> probWin; 
					if (probWin < 0.5 || probWin > 99) {
						throw std::runtime_error("problem with prob win");
					}
					//got our probability to win, now need to read hand;
					for (int i = 0; i < 5; i++) {
						fin >> token;
					}
					std::string heroHand;
					fin >> heroHand;
					//check that our range contains only 1 hand;
					fin >> token;
					if (token != "}") {
						throw std::runtime_error("problem with player number");
					}
					resultMap[heroHand] = probWin;
				}
			}
		}
		if (resultMap.size() != 169) {
			throw std::runtime_error("problem with player number, size is " + resultMap.size());
		}
		return resultMap;
	}
}
int ProbToWinWithCard::countInAIForStringInts(std::string s) {
	int inAI = 0;
	for (char c : s) {
		if (c == '1')
			++inAI;
	}
	return inAI;
}

std::string ProbToWinWithCard::PosInAIFromStringInts(std::string strIntsAI) {
	bool firstInAI = true;
	std::string PositionsInAI;
	for (int i = 1; i < totalPlayers + 1; i++) {
		if (strIntsAI[i] == '1') {
			if (!firstInAI)
				PositionsInAI += "_";
			PositionsInAI += positionName[ totalPlayers - i]; //becouse array strats from UTG action;
			firstInAI = false;
		}
	}
	return PositionsInAI;
	//return ("_" + strIntsAI);
}

void ProbToWinWithCard::parseAllPstove() {
	for (std::string strIntAI : vintFinalSituatM1AI) {
		std::string posInAI = PosInAIFromStringInts(strIntAI);
		for (int heroPos = 0; heroPos < totalPlayers; heroPos++) {
			if (strIntAI[totalPlayers - heroPos] == '0') //player folded and we don't nedd to read his equity in AI;
				continue;
			std::string strHeroPos = positionName[heroPos];
			std::string fileName = "win_probability_hand\\" + posInAI + "\\" + "p_win_" + strHeroPos + ".txt";
			int inAI = countInAIForStringInts(strIntAI);
			std::map<std::string, float> probWin = parsePstoveOneFile(fileName, inAI - 1);
			auto pairSituatPos = std::make_pair(posInAI, strHeroPos);
			cout << strIntAI <<"  " << posInAI << "  " << strHeroPos << "  " << probWin["22"] << "  " << probWin["AKs"] << endl;
			SituatAndPosToCardsAndProbs[pairSituatPos] = probWin;
		}
	}
}

map<string, float> ProbToWinWithCard::readFreqAfterPreactions() {
	map<string, float> freqAfterPreactions;
	ifstream inputFile("freq4max.txt");
	if (!inputFile.is_open()) {
		cout << "file freq4max open error" << endl;
		throw runtime_error("file freq4max open error");
	}
	else {
		std::string preactions;
		float freq;
		while (!inputFile.eof()) {
			inputFile >> preactions;
			inputFile >> freq;
			freqAfterPreactions[preactions] = freq/100;
		}
	}
	return freqAfterPreactions;
}

float ProbToWinWithCard::calc1_preFreq(const string situation, map<string, float>& freqDecision) {
	float preFrequency = 1;
	for (size_t lenOfSubstring = 1; lenOfSubstring < situation.length(); ++lenOfSubstring) {
		float preSituationAIFreq = freqDecision[situation.substr(0, lenOfSubstring)];
		if (situation[lenOfSubstring] == '1')
			preFrequency *= preSituationAIFreq;
		else
			preFrequency *= 1 - preSituationAIFreq;
	}
	return preFrequency;
}

vector<string> ProbToWinWithCard::calc1_formPostsituations(const string& situation) {
	vector<string> postSituations;
	string situationHeroAI = situation + "1"; //hero goes AI
	int playersLeftToMakeTurn = totalPlayers - (int) situationHeroAI.length() + 1;

	if (playersLeftToMakeTurn == 0) {
		postSituations = { "" };
	}
	if (playersLeftToMakeTurn == 1) {
		postSituations = { "0", "1" };
	}
	if (playersLeftToMakeTurn == 2) {
		postSituations = { "00", "01", "10", "11" };
	}
	if (playersLeftToMakeTurn == 3) {
		postSituations = { "000", "010", "100", "110","001", "011", "101", "111" };
	}
	for (vector<string>::iterator it = postSituations.begin(); it != postSituations.end(); ++it) {
		*it = situationHeroAI + *it;
	}
	for (vector<string>::iterator it = postSituations.begin(); it != postSituations.end(); ++it) {
		if (countInAIForStringInts(*it) == 1) {
			postSituations.erase(it);
			break;
		}
	}
	return postSituations;
}

float ProbToWinWithCard::calc1_postFreq(const string& postS, const string& situation, const map<string, float>& freqDecision) {
	float postFreq = 1;
	for (size_t lenOfSubstring = situation.length()+1; lenOfSubstring < postS.length(); ++lenOfSubstring) {
		float actionFreq = freqDecision.at(postS.substr(0, lenOfSubstring));
		if (postS[lenOfSubstring] == '1')
			postFreq *= actionFreq;
		else
			postFreq *= ((float)1 - actionFreq);
	}
	return postFreq;
}
void ProbToWinWithCard::calc1_AddingTo_ifd_probToWinWithCard(const string& situation, const string& postS, infoForDecision& ifd) {
	int playersLeftToMakeTurn = totalPlayers - (int) situation.length();
	std::string strHeroPos = positionName[playersLeftToMakeTurn];
	std::string actionsInReadebleFormat = PosInAIFromStringInts(postS);
	map<string, float>* itsProbForCards = &SituatAndPosToCardsAndProbs[{actionsInReadebleFormat, strHeroPos}];
	for (auto comboProb : *itsProbForCards) {
		ifd.probToWinWithCardPostHeroAI[comboProb.first] += ifd.postSituationsAndPostFreq[postS] * comboProb.second;
	}
}

void ProbToWinWithCard::calc1_probToWinWithComboPostHeroAI() {
	map<string, float> freqDecision = readFreqAfterPreactions();
	//map<string, infoForDecision> DecisionsAndItsInfo;
	for (auto &elem : freqDecision) {
		infoForDecision ifd;
		ifd.freq = elem.second;
		string situation = elem.first;
		ifd.preFreq = calc1_preFreq(situation, freqDecision);
		vector<string> postSituations = calc1_formPostsituations(situation);

		//find postFrequences for each postSituation;
		for (string postS : postSituations) {			
			ifd.postSituationsAndPostFreq[postS] = calc1_postFreq(postS, situation, freqDecision);
		}

		// know all postsituations and their probs --> need to calculate probToWinWithCardPostHeroAI;
		for (string postS : postSituations) {
			calc1_AddingTo_ifd_probToWinWithCard(situation, postS, ifd);
		}
		//save this;
		DecisionsAndItsInfo[situation] = ifd;

	}
}



void ProbToWinWithCard::LoadFileToString(const char* ReadFilePath, std::string& buffer)
{
	std::ifstream is(ReadFilePath, std::ios::binary);
	is.seekg(0, std::ios::end);
	auto length = is.tellg(); //wrong length!; It can be bigger than file size;

	if (length > 0)
	{
		buffer.resize(static_cast<std::string::size_type>(length));
		is.seekg(0);
		is.read(&buffer.front(), length);
	}
	else
	{
		std::cerr << "Ne ydalos otkrut file libo file pystoj " << ReadFilePath << std::endl;
		char a;
		std::cin >> a;
	}
}

string ProbToWinWithCard::tableNameTo01String(const string& tableName) {
	if (tableName == "CO raises 800") return "_";
	if (tableName == "BU calls 800 against CO") return "_1";
	if (tableName == "SB calls 750 against CO, BU") return "_11";
	if (tableName == "BB calls 700 against CO, BU, SB") return "_111";
	if (tableName == "BB calls 700 against CO, BU") return "_110";
	if (tableName == "SB calls 750 against CO") return "_10";
	if (tableName == "BB calls 700 against CO, SB") return "_101";
	if (tableName == "BB calls 700 against CO") return "_100";
	if (tableName == "BU raises 800") return "_0";
	if (tableName == "SB calls 750 against BU") return "_01";
	if (tableName == "BB calls 700 against BU, SB") return "_011";
	if (tableName == "BB calls 700 against BU") return "_010";
	if (tableName == "SB raises 800") return "_00";
	if (tableName == "BB calls 700 against SB") return "_001";

	cout << "error table name to 01 format converting";
	int a;
	cin >> a;
	return "ERROR";
}

void ProbToWinWithCard::readTablesWithEVs() {
	vector<tableWithEVs> vTableWithEV;
	const char* readFilePath = "step_2_after.html";
	string multiline;
	LoadFileToString(readFilePath, multiline);
	
	size_t position = 0;
	while (multiline.find("Range: ", position) != string::npos) {
		tableWithEVs tableEV;
		size_t posTableNameStart = multiline.find("Range: ", position) + 7;
		size_t posTableNameEnd = multiline.find("<", posTableNameStart);
		tableEV.tableName = multiline.substr(posTableNameStart, posTableNameEnd - posTableNameStart);
		tableEV.tableNameBy01 = tableNameTo01String(tableEV.tableName);
		for (int i = 0; i < 169; i++) {
			position = multiline.find("<td class=", position); //start of combo info;
			size_t positionFreqStart = multiline.find("<pl>", position) + 4;
			size_t positionFreqEnd = multiline.find("</pl>", positionFreqStart);
			string comboFreq = multiline.substr(positionFreqStart, positionFreqEnd - positionFreqStart - 1);

			size_t positionComboStart = multiline.find("<br/>", positionFreqEnd) + 5;
			size_t positionComboEnd = multiline.find("<br/>", positionComboStart);
			string comboName = multiline.substr(positionComboStart, positionComboEnd - positionComboStart);

			size_t positionEvStart = multiline.find("<ev>", positionComboEnd) + 4;
			size_t positionEvEnd = multiline.find("</ev>", positionComboStart);
			string comboEv = multiline.substr(positionEvStart, positionEvEnd - positionEvStart);
			position = positionEvEnd;
		
			tableEV.comboEV[comboName] = stof(comboEv);
			tableEV.comboFreqToPlay[comboName] = stof(comboFreq) / 100;

		}
		vTableWithEV.push_back(tableEV);
	}

	TablesWithEV = vTableWithEV;
}

void ProbToWinWithCard::fillcombosEvProbGlobal() {
	//create empty vectors for each hand;
	infoForDecision ifd = DecisionsAndItsInfo["_"];
	for (pair<string, float> elem : ifd.probToWinWithCardPostHeroAI) {
		vector<comboInfoFixedSituation> emptyVector;
		combosEvProbGlobal[elem.first] = emptyVector;
	}

	for (tableWithEVs table : TablesWithEV) {
		for (auto pairComboEv : table.comboEV) {
			comboInfoFixedSituation comboSitInfo;
			comboSitInfo.situationDescription_01 = table.tableNameBy01;
			comboSitInfo.situationDescription = table.tableName;
			comboSitInfo.evOfAI = pairComboEv.second;
			comboSitInfo.preFreq = DecisionsAndItsInfo[comboSitInfo.situationDescription_01].preFreq;
			comboSitInfo.probToWinIfAI = DecisionsAndItsInfo[comboSitInfo.situationDescription_01].probToWinWithCardPostHeroAI[pairComboEv.first];
			comboSitInfo.combo = pairComboEv.first;

			combosEvProbGlobal[pairComboEv.first].push_back(comboSitInfo);
		}
	}

	//add situation "Hero BB, all fold":
	//find freq of all fold situation;
	float probAllFold = 1;
	for (auto decision : DecisionsAndItsInfo) {
		if (decision.first.size() == totalPlayers) //BB action;
			probAllFold -= decision.second.preFreq;
	}
	//add situation "hero BB all fold" to all combo list;
	for (pair<string, float> elem : ifd.probToWinWithCardPostHeroAI) { //for all combos;
		comboInfoFixedSituation	HeroBbAllFold;
		HeroBbAllFold.situationDescription = "Hero BB all fold";
		HeroBbAllFold.situationDescription_01 = "_000";
		HeroBbAllFold.evOfAI = 0.5;
		HeroBbAllFold.preFreq = probAllFold;
		HeroBbAllFold.probToWinIfAI = 0; //we never close matrix cells in all fold case;
		HeroBbAllFold.combo = elem.first;
		combosEvProbGlobal[elem.first].push_back(HeroBbAllFold);
	}
}

