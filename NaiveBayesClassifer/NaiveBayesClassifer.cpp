#include "NaiveBayesClassifer.h"

ClassInfoNode::ClassInfoNode (void)
{
}

ClassInfoNode::~ClassInfoNode (void)
{
}

const double ClassInfoNode::dFalseProbVal = -1;

void ClassInfoNode::Clear (void)
{
	dPriProb = dFalseProbVal;
	PostProbVec.clear ();
}

NaiveBayesClassifer::NaiveBayesClassifer(void)
{
}

NaiveBayesClassifer::~NaiveBayesClassifer(void)
{
}

bool NaiveBayesClassifer::Init (const char* sFileLex)
{
	if (!toWordList.LoadWordListFrmTxt (sFileLex))
		return false;
	if (!toSegment.Init (sFileLex))
		return false;

	return true;
}

bool NaiveBayesClassifer::LoadFeatureForClass (const char *sFileFea)
{
	ifstream in (sFileFea);
	if (!in)
	{
		cerr << "can not open the file" << endl;
		return false;
	}

	// 1. get the tag set
	string sLine;
	getline (in, sLine);
	istringstream isLine (sLine);
	ClassInfoNode theNode;
	while (isLine >> theNode.sTag)
		ClassInfoVec.push_back (theNode);

	// 2. get the prior probability
	getline (in, sLine);
	isLine.clear();
	isLine.rdbuf()->str(sLine);
	isLine.seekg (0, ios::beg); // ! reset the write position
	vector<ClassInfoNode>::iterator pClass = ClassInfoVec.begin();
	while (pClass != ClassInfoVec.end())
	{
		if (!(isLine >> pClass->dPriProb))
		{
			cerr << "fail to get the prior probability" << endl;
			return false;
		}
		pClass++;
	}

	// 3. allocate memory for word feature vector
	int iNum = toWordList.GetWordNum ();
	if (iNum <= 0)
	{
		cerr << "error in wordlist" << endl;
		return false;
	}
	pClass = ClassInfoVec.begin();
	while (pClass != ClassInfoVec.end())
	{
		pClass->PostProbVec.resize (iNum, ClassInfoNode::dFalseProbVal);
		pClass++;
	}

	// 4. get the post probability
	while (getline (in, sLine))
	{
		isLine.clear();
		isLine.rdbuf()->str (sLine);
		isLine.seekg (0, ios::beg);
		string sWord;
		isLine >> sWord;
		int iWordId;
		if (toWordList.GetWordIdFrmStr (sWord, iWordId))
		{
			pClass = ClassInfoVec.begin();
			while (pClass != ClassInfoVec.end())
			{
				isLine >> pClass->PostProbVec.at(iWordId);
				pClass++;
			}
		}
	}

	return true;
}

bool NaiveBayesClassifer::ClassifySentence (string &sSenStr, string &sTag)
{
	// 1. segment
	vector<string> WordVec;
	if (!toSegment.SegmentSentence (sSenStr, WordVec))
		return false;

	vector<int> WordIdVec;
	toWordList.GetWordIdVecFrmStrVec (WordVec, WordIdVec);

	// 2. calculate the probability of each class
	int iClassNum = ClassInfoVec.size();
	vector<double> ProbVec (iClassNum, 0.0);

	bool bAbleClassify = false;
	vector<ClassInfoNode>::iterator pClass = ClassInfoVec.begin ();
	vector<double>::iterator pProb = ProbVec.begin();
	while (pClass != ClassInfoVec.end () && pProb != ProbVec.end())
	{
		double dTmpProb = 0.0;
		double dTmpProbLog = 0.0;
		vector<int>::iterator pWordId = WordIdVec.begin ();
		while (pWordId != WordIdVec.end())
		{
			dTmpProb = pClass->PostProbVec.at (*pWordId);
			// if (dTmpProb > 0)
			if (dTmpProb > 1e3 * DBL_MIN)
			{
				bAbleClassify = true;
				dTmpProbLog += log (dTmpProb + DBL_MIN);
			}
			pWordId++;
		}
		dTmpProbLog += log (pClass->dPriProb + DBL_MIN);
		*pProb = dTmpProbLog;
		pClass++; pProb++;
	}

	if (!bAbleClassify)
		return false;

	// 3. select the class with max prob
	double dMaxProbLog = -DBL_MAX;
	int iClassId = -1;
	int iTmpClassId = 0;
	pProb = ProbVec.begin ();
	while (pProb != ProbVec.end())
	{
		if (dMaxProbLog < *pProb)
		{
			dMaxProbLog = *pProb;
			iClassId = iTmpClassId;
		}
		pProb++; iTmpClassId++;
	}

	if (-1 == iClassId)
		return false;
	else
		sTag = ClassInfoVec.at (iClassId).sTag;

	return true;
}

bool NaiveBayesClassifer::ClassifySenInCorpus (const char* sFileIn, const char* sFileLog)
{
	ifstream in (sFileIn);
	ofstream out (sFileLog);
	if (!in || !out)
	{
		cerr << "Can not open the files" << endl;
		return false;
	}

	string sLine;
	int i = 0;
	while (getline (in, sLine))
	{
		// show something on screen
		if (0 == i%1000)
			cout << i << "\n";
		i++;

		string sTag;
		if (ClassifySentence (sLine, sTag))
		{
			out << sTag << " " << sLine;
		}
		out << "\n";
	}

	return true;
}

void NaiveBayesClassifer::Test (void)
{
	//Init ("..\\..\\Data\\Lexicon\\Word_CoreDict.txt");
	/*LoadFeatureForClass ("..\\..\\Data\\Sex_Weibo_Fea.txt");
	ClassifySenInCorpus ("..\\..\\Data\\SexCorpus\\Sex_20120507.txt",
		"..\\..\\Data\\TagRslt\\Tag_Sex_20120507.txt");
	ClassifySenInCorpus ("..\\..\\Data\\WeiboCorpus\\weibo_20120801_12M_18M.main.txt",
		"..\\..\\Data\\TagRslt\\Tag_weibo_20120801_12M_18M.main.txt");*/

	//LoadFeatureForClass ("..\\..\\Data\\Sex_Weibo_Fea_2w.txt");
	/*ClassifySenInCorpus ("..\\..\\Data\\SexCorpus\\Sex_20120507.txt",
		"..\\..\\Data\\TagRslt\\Tag_2w_Sex_20120507.txt");
	ClassifySenInCorpus ("..\\..\\Data\\WeiboCorpus\\weibo_20120801_12M_18M.main.txt",
		"..\\..\\Data\\TagRslt\\Tag_2w_weibo_20120801_12M_18M.main.txt");*/

	// Init ("..\\..\\Data\\Lexicon\\Word_CoreDict.txt");
	// LoadFeatureForClass ("..\\..\\Data\\Rubbish_Weibo_Fea_2w.txt");
	// LoadFeatureForClass ("..\\..\\Data\\Rubbish_Weibo_Fea_3w.txt");

	/*ClassifySenInCorpus ("..\\..\\Data\\WeiboCorpus\\weibo_20120801_12M_18M.main.txt",
		"..\\..\\Data\\TagRslt\\Tag_2w_weibo_20120801_12M_18M.main.txt");*/
	/*ClassifySenInCorpus ("..\\..\\Data\\Rubbish\\Rubbish_v4.txt",
		"..\\..\\Data\\TagRslt\\TagRubbish_v4.txt");*/

	/*Init ("..\\..\\Data\\Lexicon\\Word_CoreDict_DirtyWords.txt");
	LoadFeatureForClass ("..\\..\\Data\\Rubbish_Weibo_Fea2_3w.txt");
	ClassifySenInCorpus ("..\\..\\Data\\Rubbish\\Rubbish_v4.txt",
		"..\\..\\Data\\TagRslt\\Tag_Rubbish2_v4.txt");
	ClassifySenInCorpus ("..\\..\\Data\\WeiboCorpus\\weibo_20120801_12M_18M.main.txt",
		"..\\..\\Data\\TagRslt\\Tag2_3w_weibo_20120801_12M_18M.main.txt");*/

	/*Init ("..\\..\\Data\\Lexicon\\Word_CoreDict_DirtyWords.txt");
	LoadFeatureForClass ("..\\..\\Data\\Rubbish_Weibo_Fea2_5w.txt");*/
	/*ClassifySenInCorpus ("..\\..\\Data\\Rubbish\\Rubbish_v4.txt",
		"..\\..\\Data\\TagRslt\\Tag_Rubbish2_v5.txt");*/
	/*ClassifySenInCorpus ("..\\..\\Data\\Rubbish\\Rubbish_v4_clear.txt",
		"..\\..\\Data\\TagRslt\\Tag_Rubbish2_clearv5.txt");*/
	/*ClassifySenInCorpus ("..\\..\\Data\\WeiboCorpus\\weibo_20120801_12M_18M.main.txt",
		"..\\..\\Data\\TagRslt\\Tag2_5w_weibo_20120801_12M_18M.main.txt");*/

	/*Init ("..\\..\\Data\\Lexicon\\Word_CoreDict_DirtyWords.txt");
	LoadFeatureForClass ("..\\..\\Data\\Rubbish_Weibo_Fea2_7w.txt");
	ClassifySenInCorpus ("..\\..\\Data\\Rubbish\\Rubbish_v4_clear.txt",
		"..\\..\\Data\\TagRslt\\Tag_Rubbish2_clearv6.txt");
	ClassifySenInCorpus ("..\\..\\Data\\WeiboCorpus\\weibo_20120801_12M_18M.main.txt",
		"..\\..\\Data\\TagRslt\\Tag2_7w_weibo_20120801_12M_18M.main.txt");*/
	Init ("..\\..\\Data\\Lexicon\\Word_CoreDict_DirtyWords.txt");
	LoadFeatureForClass ("..\\..\\Data\\Rubbish_Weibo_Fea2_7w.txt");
	ClassifySenInCorpus ("..\\..\\Data\\Rubbish\\Rubbish_v4_clear.txt",
		"..\\..\\Data\\TagRslt\\Tag_Rubbish2_clearv6_try.txt");

	/*Init ("..\\..\\Data\\Lexicon\\Word_CoreDict17w_DirtyWords.txt");
	LoadFeatureForClass ("..\\..\\Data\\Rubbish_Weibo_Fea3_7w.txt");
	ClassifySenInCorpus ("..\\..\\Data\\Rubbish\\Rubbish_v4_clear.txt",
		"..\\..\\Data\\TagRslt\\Tag_Rubbish3_clearv6.txt");
	ClassifySenInCorpus ("..\\..\\Data\\WeiboCorpus\\weibo_20120801_12M_18M.main.txt",
		"..\\..\\Data\\TagRslt\\Tag3_7w_weibo_20120801_12M_18M.main.txt");*/

	/*Init ("..\\..\\Data\\Lexicon\\Word_CoreDict17w_DirtyWords.txt");
	LoadFeatureForClass ("..\\..\\Data\\Rubbish_Weibo_Fea3_2_7w.txt");
	ClassifySenInCorpus ("..\\..\\Data\\Rubbish\\Rubbish_v4_clear.txt",
		"..\\..\\Data\\TagRslt\\Tag_Rubbish3_2_clearv6.txt");*/
	/*ClassifySenInCorpus ("..\\..\\Data\\WeiboCorpus\\weibo_20120801_12M_18M.main.txt",
		"..\\..\\Data\\TagRslt\\Tag3_7w_weibo_20120801_12M_18M.main.txt");*/

	/*Init ("..\\..\\Data\\Lexicon\\Word_CoreDict17w_DirtyWords.txt");
	LoadFeatureForClass ("..\\..\\Data\\Rubbish_Weibo_Fea3_0w.txt");
	ClassifySenInCorpus ("..\\..\\Data\\Rubbish\\Rubbish_v4_clear.txt",
		"..\\..\\Data\\TagRslt\\Tag_Rubbish3_clearv7.txt");*/
}
