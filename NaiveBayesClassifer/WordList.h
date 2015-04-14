#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

class WordList
{
private:
	// store the lexicon
	vector<string> WordVec;

public:
	// load word list from txt file
	bool LoadWordListFrmTxt (const char* sFileIn);
	// judge whether the lexicon is empty
	bool IsEmpty (void);
	// get the item number in lexicon
	int GetWordNum (void);
	// check whether it is a word string in lexicon
	bool IsWord (string &sWord);
	// get the word index from string, return false if there is no such a word
	bool GetWordIdFrmStr (string &sWord, int &iWordIndex);
	// get the word index vector from string vector
	void GetWordIdVecFrmStrVec (vector<string> &WordVec, vector<int> &WordIdVec);
	// get the word string from index, return false if there is no such a word
	bool GetWordStrFrmId (int iWordIndex, string &sWord);
	// get the word string vector from index vector
	void GetWordStrVecFrmIdVec (vector<int> &WordIdVec, vector<string> &WordVec);

	WordList(void);
	~WordList(void);
};
