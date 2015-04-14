#include "WordList.h"

WordList::WordList(void)
{
}

WordList::~WordList(void)
{
}

bool WordList::LoadWordListFrmTxt (const char* sFileIn)
{
	ifstream in (sFileIn);
	if (!in)
	{
		cerr << "Can not open the file!" << endl;
		return false;
	}

	WordVec.clear();

	string sWord;
	while (in >> sWord)
		WordVec.push_back (sWord);

	sort (WordVec.begin(), WordVec.end());

	return true;
}

bool WordList::IsEmpty (void)
{
	return WordVec.empty();
}

int WordList::GetWordNum (void)
{
	return WordVec.size ();
}

bool WordList::IsWord (string &sWord)
{
	return binary_search (WordVec.begin(), WordVec.end(), sWord);
}

bool WordList::GetWordIdFrmStr (string &sWord, int &iWordIndex)
{
	vector<string>::iterator p = lower_bound (WordVec.begin(), WordVec.end(), sWord);
	if (p != WordVec.end() && *p == sWord)
	{
		iWordIndex = (int)(p - WordVec.begin());
		return true;
	}
	else
		return false;
}

void WordList::GetWordIdVecFrmStrVec (vector<string> &WordVec, vector<int> &WordIdVec)
{
	vector<string>::iterator p = WordVec.begin ();
	while (p != WordVec.end())
	{
		int iWordIndex = -1;
		if (GetWordIdFrmStr (*p, iWordIndex))
			WordIdVec.push_back (iWordIndex);
		p++;
	}
}

bool WordList::GetWordStrFrmId (int iWordIndex, string &sWord)
{
	int iNum = (int)WordVec.size();
	if (iWordIndex < 0 || iWordIndex >= iNum)
		return false;
	else
	{
		sWord = WordVec[iWordIndex];
		return true;
	}
}

void WordList::GetWordStrVecFrmIdVec (vector<int> &WordIdVec, vector<string> &WordVec)
{
	vector<int>::iterator p = WordIdVec.begin ();
	while (p != WordIdVec.end())
	{
		string sWord;
		if (GetWordStrFrmId (*p, sWord))
			WordVec.push_back (sWord);
		p++;
	}
}
