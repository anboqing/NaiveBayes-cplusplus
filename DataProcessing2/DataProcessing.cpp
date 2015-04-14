#include "DataProcessing.h"

void DataProcessing::FilterStopWordsInVec(std::vector<string> & WordVec)
{
	vector<string>::iterator pWord = WordVec.begin();
	int iOldSize = (int)WordVec.size();
	int iNewSize = 0;
	while (pWord != WordVec.end())
	{
		if (pStopWordList->IsItem (*pWord))
			pWord = WordVec.erase (pWord);
		else
		{
			pWord++;
			iNewSize++;
		}
	}
	if (iNewSize != iOldSize)
		WordVec.resize (iNewSize);
}

DataProcessing::DataProcessing(void)
{
	pWordList = NULL;
	pStopWordList = NULL;
	pSegmenter = NULL;
}

DataProcessing::~DataProcessing(void)
{
	delete pWordList;
	delete pStopWordList;
	delete pSegmenter;
}

bool DataProcessing::Init (const char * sFileLex, const char * sFileStopWords)
{
	if (pWordList != NULL)
		delete pWordList;
	pWordList = new ItemList;
	if (!pWordList->LoadItemListFrmTxt (sFileLex))
		return false;

	if (pStopWordList != NULL)
		delete pStopWordList;
	pStopWordList = new ItemList;
	if (!pStopWordList->LoadItemListFrmTxt (sFileStopWords))
		return false;

	if (pSegmenter != NULL)
		delete pSegmenter;
	pSegmenter = new Segment;
	pSegmenter->Init (pWordList);
	return true;
}

bool DataProcessing::CorpusProcessing(const char *sFileIn, string & sSegmenter, const char *sFileOut)
{
	ifstream in (sFileIn, ios_base::binary);
	ofstream out (sFileOut);

	string sLine;
	int i = 0;
	while (getline (in, sLine))
	{
		// show some information on screen
		if (0 == i%10000)
			cout << i << "\n";
		i++;

		// 1. the class index
		string::size_type i = sLine.find (sSegmenter);
		if (string::npos == i)
			continue;
		string sTmp = sLine.substr (0, i);
		int iClassId = atoi (sTmp.c_str());

		// 2. the rest sentence
		i += sSegmenter.length();
		sTmp = sLine.substr (i);

		// 3. segment
		vector<string> WordVec;
		if (!pSegmenter->SegmentSentence (sTmp, WordVec))
			continue;

		// 4. filter the words in stop word list
		FilterStopWordsInVec (WordVec);

		// 5. convert to the word index vector
		vector<int> WordIdVec;
		pWordList->GetItemIdVecFrmStrVec (WordVec, WordIdVec);

		// 6. dump it into txt file
		out << iClassId << ":";
		copy (WordIdVec.begin(), WordIdVec.end(), ostream_iterator<int> (out, " "));
		out << "\n";
	}

	return true;
}

void DataProcessing::Test (void)
{
	Init ("..\\..\\Data\\Lexicon\\Word_CoreDict_DirtyWords.txt", 
		"..\\..\\Data\\Lexicon\\stopwordlist_sort.txt");
	/*CorpusProcessing ("..\\..\\Data\\ForDataProcessing2\\Train.txt", string("&&&&&&&"),
		"..\\..\\Data\\ForDataProcessing2\\Train_Format.txt");*/
	CorpusProcessing ("..\\..\\Data\\ForDataProcessing2\\Test.txt", string("&&&&&&&"),
		"..\\..\\Data\\ForDataProcessing2\\Test_Format.txt");
	/*CorpusProcessing ("..\\..\\Data\\Rubbish\\Rubbish_v4_clearTag.txt", string("&&&&&&&"),
		"..\\..\\Data\\ForDataProcessing2\\Test_Rubbishv4_Format.txt");*/
	/*CorpusProcessing ("..\\..\\Data\\WeiboCorpus\\weibo_20120801_12M_18M.mainTag.txt", string("&&&&&&&"),
		"..\\..\\Data\\ForDataProcessing2\\Test_weibo_Format.txt");*/
}