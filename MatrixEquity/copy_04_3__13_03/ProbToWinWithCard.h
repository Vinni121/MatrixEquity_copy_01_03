#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <exception>
#include <map>
#include <vector>
#include <set>
using namespace std;

struct infoForDecision {
	float freq = 0;
	float preFreq = 0;
	map<string, float> postSituationsAndPostFreq;
	map<string, float> probToWinWithCardPostHeroAI;
};

struct tableWithEVs {
	string tableName;
	string tableNameBy01;
	map<string, float> comboEV;
	map<string, float> comboFreqToPlay;
};

struct comboInfoFixedSituation {
	float evOfAI = 0;
	float preFreq = 0;
	float probToWinIfAI = 0;
	string combo;
	string situationDescription;
	string situationDescription_01;
	int const operator<(const comboInfoFixedSituation& rhd);
};

class ProbToWinWithCard
{
public:
	ProbToWinWithCard();
	map<string, vector<comboInfoFixedSituation>> combosEvProbGlobal; //all situations for 1 combo;
	int totalPlayers;

private:

	//read pstove probs
	void parseAllPstove();
	string PosInAIFromStringInts(string);
	int countInAIForStringInts(string);
	map<string, float> parsePstoveOneFile(string FileName, int numberOfPalyerToRead);
	vector<string> positionName;
	vector<string> vintFinalSituatM1AI;
	vector<string> vsFinalSituationsMore1AI;
	map <pair<string,string>, map<string, float>> SituatAndPosToCardsAndProbs; //from playersInAIPositions + playerPosition --> map from combo to probability
	
	//for calculating probability to win with fixed combo after hero AI (for all situations);
	map<string, infoForDecision> DecisionsAndItsInfo;
	map<string, float> readFreqAfterPreactions();
	float calc1_preFreq(const string situation, map<string, float>& freqDecision);
	vector<string> calc1_formPostsituations(const string& situation);
	float calc1_postFreq(const string& postS, const string& situation, const map<string, float>& freqDecision);
	void calc1_AddingTo_ifd_probToWinWithCard(const string& situation, const string& postS, infoForDecision& ifd);
	void calc1_probToWinWithComboPostHeroAI();

	//read Tables With EVs
	vector<tableWithEVs> TablesWithEV;
	void readTablesWithEVs();
	void LoadFileToString(const char* ReadFilePath, std::string& buffer);
	string tableNameTo01String(const string& tableName);

	//create arrays with probs for decision situations;
	void fillcombosEvProbGlobal();



};

