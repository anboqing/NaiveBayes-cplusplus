#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

class ItemList
{
private:
	// store the lexicon
	vector<string> ItemVec;

public:
	// load Item list from txt file
	bool LoadItemListFrmTxt (const char* sFileIn);
	// judge whether the lexicon is empty
	bool IsEmpty (void);
	// get the item number in lexicon
	int GetItemNum (void);
	// check whether it is a Item string in lexicon
	bool IsItem (string &sItem);
	// get the Item index from string, return false if there is no such a Item
	bool GetItemIdFrmStr (string &sItem, int &iItemIndex);
	// get the Item index vector from string vector
	void GetItemIdVecFrmStrVec (vector<string> &ItemVec, vector<int> &ItemIdVec);
	// get the Item string from index, return false if there is no such a Item
	bool GetItemStrFrmId (int iItemIndex, string &sItem);
	// get the Item string vector from index vector
	void GetItemStrVecFrmIdVec (vector<int> &ItemIdVec, vector<string> &ItemVec);

	ItemList(void);
	~ItemList(void);
};
