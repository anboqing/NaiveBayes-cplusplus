#pragma once

/***********************************************************************************
* The data processer for naive bayes classifier, version 0.01
*
* Implemented by Jinghui Xiao (xiaojinghui@gmail.com or xiaojinghui1978@qq.com)
* Last updated on 2012
***********************************************************************************/

#include "ItemList.h"
#include "Segment.h"

#include <string>
#include <vector>

using namespace std;

class DataProcessing
{
public:
	DataProcessing(void);
	~DataProcessing(void);

	// initialize pWordList and pSegmenter
	bool Init (const char * sFileLex, const char * sFileStopWords);
	// process the corpus into the specific format required by naive bayes
	//		input format: ClassIndex Segmenter Sentence
	//		output format: ClassIndex Segmenter WordIndex1 space WordIndex2...
	bool CorpusProcessing (const char * sFileIn, string & sSegmenetr, const char * sFileOut);

	// process the corpus here
	void Test (void);

private:
	ItemList * pWordList;
	ItemList * pStopWordList;
	Segment * pSegmenter;

	// filer the word string vector by stop word list
	void FilterStopWordsInVec (vector<string> & WordVec);
};
