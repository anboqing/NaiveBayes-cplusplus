#pragma once

/***********************************************************************************
* The training class for naive bayes classifier, version 0.01
* It provides the functions of feature selection and post-probabiltiy stat
*
* Implemented by Jinghui Xiao (xiaojinghui@gmail.com or xiaojinghui1978@qq.com)
* Last updated on 2012
***********************************************************************************/

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <functional>
#include <cfloat>

using namespace std;


class NaiveBayes
{
public:
	NaiveBayes(void);
	~NaiveBayes(void);

	// Train the naive bayes model
	//
	// The format of input samples:
	//		ClassLabelIndex segmenter(not whitespace) ItemOneIndex whitespace ItemTwoIndex......
	//	iClassNum: the number of class label index, [0, iClassNum-1]
	//	iFeaTypeNum: the number of feature type index, [0, iFeaTypeNum-1]
	//	sSegmenter: the segmenter between Class label index and Item index
	//	iFeaExtractNum: the number of features which is going to extract
	//	sFileModel: the output model into txt file
	//	bCompactModel: whether to show some infor for debug, true for not include those infor
	// 
	// The format of compact model parameters 
	//		1. the number of class
	//		2. the prior probability of class
	//		3. the conditional probability of p(item|class)
	bool Train (const char * sFileSample, int iClassNum, int iFeaTypeNum, 
		string & sSegmenter, int iFeaExtractNum, const char * sFileModel, bool bCompactModel = true);

	// Load the naive bayes model
	bool LoadNaiveBayesModel (const char * sFileModel);

	// predict according to the input features
	bool PredictByInputFeas (vector<int> & FeaIdVec, int & iClassId);

	// predict by input test corpus whose format is the same with the training corpus
	bool PredictFrmTstCorpus (const char * sFileTestCorpus, string & sSegmenter, const char * sFileOutput);

	// do the dirty works
	void Test (void);

private:
	// function member
	// the node for feature and its post probability in current class
	struct FeaProbNode
	{
		int iFeadId;
		double dProb;
	};
	// the node for class and its features
	struct ClassFeaNode
	{
		int iClassId;
		double dClassProb;	// the prior probability of a class
		vector<FeaProbNode> FeaVec;
	};
	// the vector of ClassFeaNode, it contains all the necessary infor to predict a new sample
	vector<ClassFeaNode> ClassFeaVec;

	// Feature selection by chi-square, called by Train
	void FeaSelByChiSquare (
		double ** ppChiMatrix,	// the input chi square matrix
		double ** ppPProbMatrix,// the input sample-class matrix
		int iClassNum,			// the input class number
		int iFeaTypeNum,		// the input feature type number
		int iSelFeaNum,			// the input total number of selected feature
		int * pFeaSelected);	// the output feature type selected array
	// Calculate the log probability value by a feature index function
	double CalcProbLogByFeaVec (vector<int> & FeaIdVec, vector<FeaProbNode> & FeaVec);
};
