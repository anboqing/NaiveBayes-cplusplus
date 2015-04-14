#pragma once

/***********************************************************************************
* The implementation for naive bayes classifier, version 0.01
* It provides the functions of classification. 
*
* Implemented by Jinghui Xiao (xiaojinghui@gmail.com or xiaojinghui1978@qq.com)
* Last updated on 2012
***********************************************************************************/

#include "WordList.h"
#include "Segment.h"

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cfloat>
#include <cmath>

using namespace std;

// the class node containing the class info,
// such as the post-probability, the prior probability...
class ClassInfoNode
{
public:
	// the default value of a false probability
	const static double dFalseProbVal;

	// the class tag
	string sTag;
	// the prior probability for this class
	double dPriProb;
	// the post probability for the features, the improve the speed,
	// the size of PostProbVec is the same as word number, so we can
	// use the hash method to find the word
	vector<double> PostProbVec;

	void Clear (void);
	ClassInfoNode(void);
	~ClassInfoNode(void);
};

// the class for the naive bayes classifier
class NaiveBayesClassifer
{
private:
	// the word index
	WordList toWordList;
	// the segmentor
	Segment toSegment;

	// the class infor node vector containing the associated features
	vector<ClassInfoNode> ClassInfoVec;

public:
	// init
	bool Init (const char* sFileLex);
	// load the feature from sFileFea and fill in ClassInfoVec
	bool LoadFeatureForClass (const char* sFileFea);
	// tag the class for a sentence string
	bool ClassifySentence (string &sSenStr, string &sTag);
	// classify all the sentences in a corpus
	bool ClassifySenInCorpus (const char* sFileIn, const char* sFileLog);

	void Test (void);

public:
	NaiveBayesClassifer(void);
	~NaiveBayesClassifer(void);
};
