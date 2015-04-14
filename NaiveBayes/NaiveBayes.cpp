#include "NaiveBayes.h"

NaiveBayes::NaiveBayes(void)
{
}

NaiveBayes::~NaiveBayes(void)
{
}

// for sort and get the top n feature type, called by FeaSelByChiSquare
class FeaChiNode
{
public:
	int iFeaId;
	double dChiVal;

	FeaChiNode()
	{
		iFeaId = -1;
		dChiVal = 0.0;
	};
	~FeaChiNode(){};

	inline bool operator > (const FeaChiNode & theNode)const 
	{
		if (dChiVal > theNode.dChiVal)
			return true;
		else
			return false;
	}
};

void NaiveBayes::FeaSelByChiSquare(double ** ppChiMatrix, double ** ppPProbMatrix, int iClassNum, 
										int iFeaTypeNum, int iSelFeaNum, int * pFeaSelected)
{
	// the number of feature to be selected for each class
	int iSelFeaNumAClass = iSelFeaNum / iClassNum;

	// for sort and get the top n feature type
	/*class FeaChiNode
	{
	public:
		int iFeaId;
		double dChiVal;

		FeaChiNode()
		{
			iFeaId = -1;
			dChiVal = 0.0;
		};
		~FeaChiNode(){};

		inline bool operator > (const FeaChiNode & theNode)const 
		{
			if (dChiVal > theNode.dChiVal)
				return true;
			else
				return false;
		}
	};*/
	vector<FeaChiNode> FeaChiVec;
	FeaChiVec.resize (iFeaTypeNum);

	// iterate ppChiMatrix and fill in ppFeaSelMatrix and pFeaSelected
	for (int i=0; i<iClassNum; i++)
	{
		// fill in FeaChiVec by ppChiMatrix[i]
		for (int j=0; j<iFeaTypeNum; j++)
		{
			FeaChiVec.at (j).iFeaId = j;
			FeaChiVec.at (j).dChiVal = ppChiMatrix[i][j];
		}
		// sort by Chi-square value
		sort (FeaChiVec.begin(), FeaChiVec.end(), greater<FeaChiNode>());

		// get the top iSelFeaNumAClass items, and set the flag arra
		vector<FeaChiNode>::iterator pFeaChi = FeaChiVec.begin();
		for (int k=0; k<iSelFeaNumAClass && pFeaChi != FeaChiVec.end(); k++, pFeaChi++)
		{
			// these conditions could be relaxed if necessary
			while (pFeaChi != FeaChiVec.end() &&		
					(
					0 != pFeaSelected[pFeaChi->iFeaId] ||			// find the feature type that never been selected
					DBL_MIN > ppPProbMatrix[i][pFeaChi->iFeaId])	// in the same time, there are sample hitting this feature type and class
					)
				pFeaChi++;

			if(pFeaChi != FeaChiVec.end())
				pFeaSelected[pFeaChi->iFeaId] = 1;
			else
				break;
		}
	}
}

double NaiveBayes::CalcProbLogByFeaVec (vector<int> & FeaIdVec, vector<FeaProbNode> & FeaVec)
{
	double dProbLog = 0.0;
	vector<int>::iterator pFeaId = FeaIdVec.begin();
	vector<FeaProbNode>::iterator pFea = FeaVec.begin();
	while (pFeaId != FeaIdVec.end() && pFea != FeaVec.end())
	{
		if (*pFeaId == pFea->iFeadId)
		{
			dProbLog += log (pFea->dProb + DBL_MIN);
			pFeaId++;
			pFea++;
		}
		else if (*pFeaId < pFea->iFeadId)	
			pFeaId++;
		else
		{
// strictly speaking, we should do this in naive bayes:
// there are two value for a feature type: 1 and 0
// It is 1 when the feature occurs in input sample; otherwise, 0.
#if 1
			dProbLog += log (1.0 - pFea->dProb + DBL_MIN);
#endif
			pFea++;
		}
	}

	return dProbLog;
}

bool NaiveBayes::Train (const char * sFileSample, int iClassNum, int iFeaTypeNum,
							 string & sSegmenter, int iFeaExtractNum, const char * sFileModel, bool bCompactModel)
{
	if (iClassNum <= 0 || iFeaTypeNum <= 0 || iFeaExtractNum <= 0)
		return false;

	ifstream in (sFileSample, ios_base::binary);
	ofstream out (sFileModel);
	if (!in || !out)
	{
		cerr << "Can not open the file" << endl;
		return false;
	}

	// 1. the temp data structure for model parameters
	// 1.1 the total number of document in training samples
	int iTotalDocNum = 0;
	// 1.2 the prior probability of class, temparaly it store the doc number in this class
	double * pClassPriorProb = new double [iClassNum];
	memset (pClassPriorProb, 0, iClassNum*sizeof(double));
	// 1.3 the prior probability of feature type, temparaly it stores the doc number in this feature
	double * pFeaItemPriorProb = new double [iFeaTypeNum];
	memset (pFeaItemPriorProb, 0, iFeaTypeNum*sizeof(double));
	// 1.4 the chi-square value that feature falls into class, temparaly it stores the doc number for this class and feature
	double ** ppChiMatrix = new double * [iClassNum];
	for (int i=0; i<iClassNum; i++)
	{
		ppChiMatrix[i] = new double [iFeaTypeNum];
		memset (ppChiMatrix[i], 0, iFeaTypeNum*sizeof(double));
	}
	// 1.5 the post-probability for class and feature
	double ** ppPProbMatrix = new double * [iClassNum];
	for (int i=0; i<iClassNum; i++)
	{
		ppPProbMatrix[i] = new double [iFeaTypeNum];
		memset (ppChiMatrix[i], 0, iFeaTypeNum*sizeof(double));
	}
	// 1.6 for the feature selection
	int * pFeaSelected = new int [iFeaTypeNum];
	memset (pFeaSelected, 0, iFeaTypeNum*sizeof(int));

	// 2. iterate the training samples and fill count into the temp data structure
	string sLine;
	int i = 0;
	while (getline (in, sLine))
	{
		// show some information on screen
		if (0 == i%10000)
			cout << i << "\n";
		i++;

		// 2.1 the total number of doc
		iTotalDocNum++;

		// 2.2 split the sample into class and feature items
		string::size_type iSeg = sLine.find_first_of (sSegmenter);
		string sTmp = sLine.substr (0, iSeg);
		int iClassId = atoi (sTmp.c_str());
		if (iClassId >= iClassNum)
			continue;
		pClassPriorProb [iClassId]++;

		// 2.3 count the rest feature items
		iSeg += sTmp.length();
		sTmp = sLine.substr (iSeg);
		istringstream isLine (sTmp);

#if 0
		// These codes take into Item Count instead of Document Count
		string sTmpItem;
		while (isLine >> sTmpItem)
		{
			int iFeaItemId = atoi (sTmpItem.c_str());
			if (iFeaItemId >= iFeaTypeNum)
				continue;
			// add the count
			pFeaItemPriorProb [iFeaItemId]++;
			ppChiMatrix [iClassId][iFeaItemId]++;

		}
		// in what next, it takes into document count of feature type
#endif

		string sTmpItem;
		vector<int> TmpFeaIdVec;
		while (isLine >> sTmpItem)
		{
			int iFeaItemId = atoi (sTmpItem.c_str());
			if (iFeaItemId >= iFeaTypeNum)
				continue;
			TmpFeaIdVec.push_back (iFeaItemId);
		}
		// sort and remove duplicated items
		sort (TmpFeaIdVec.begin(), TmpFeaIdVec.end());
		TmpFeaIdVec.erase (unique (TmpFeaIdVec.begin(), TmpFeaIdVec.end()), TmpFeaIdVec.end ());
		// add the count 
		vector<int>::iterator pTmpFeaId = TmpFeaIdVec.begin();
		while (pTmpFeaId != TmpFeaIdVec.end ())
		{
			pFeaItemPriorProb [*pTmpFeaId]++;
			ppChiMatrix [iClassId][*pTmpFeaId]++;
			pTmpFeaId++;
		}
	}

	// 3. calculate the model parameters
	// 3.1 the chi-square value as well as the post-probabilty
	for (int i=0; i<iClassNum; i++)
	{
		for (int j=0; j<iFeaTypeNum; j++)
		{
			double dA = ppChiMatrix[i][j];
			double dB = pFeaItemPriorProb[j] - dA; // currently pFeaItemPriorProb[i] == sum_i (ppChiMatrix[i][j])
			double dC = pClassPriorProb [i] - dA;  // currently pClassPriorProb[i] == sum_j (ppChiMatrix[i][j])
			double dD = (double)iTotalDocNum - dA - dB - dC;

			// the chi value 
			double dNumerator = dA * dD;
			dNumerator -= dB * dC;
			dNumerator = pow (dNumerator, 2.0);
			double dDenominator = dA + dB;
			dDenominator *= (dC + dD);
			dDenominator += DBL_MIN; // for smoothing
			ppChiMatrix[i][j] = dNumerator / dDenominator;

			// the post-probability: p(feature|class)
			ppPProbMatrix[i][j] = dA / pClassPriorProb [i];
		}
	}

	// 3.2 the prior probability of class
	for (int i=0; i<iClassNum; i++)
		pClassPriorProb [i] /= iTotalDocNum;

	// 3.3 the prior probability of feature
	for (int i=0; i<iFeaTypeNum; i++)
		pFeaItemPriorProb [i] /= iTotalDocNum;


	// 4. feature selection
	FeaSelByChiSquare (ppChiMatrix, ppPProbMatrix, iClassNum, 
		iFeaTypeNum, iFeaExtractNum, pFeaSelected);

	// 5. dump the model into txt file

	if (bCompactModel)		// output the parameters only for predicting
	{
		// 5.1 the prior probability of class
		out << iClassNum << endl;
		for (int i=0; i<iClassNum; i++)
		{
			out << pClassPriorProb [i] << "\n";
		}
		// 5.2 the actual selected feature type number
		int iActualFeaNum = 0;
		for (int j=0; j<iFeaTypeNum; j++)
		{
			if (1 == pFeaSelected[j])
				iActualFeaNum ++;
		}
		out << iActualFeaNum << endl;
		// 5.3 the post probability
		for (int i=0; i<iClassNum; i++)
		{
			for (int j=0; j<iFeaTypeNum; j++)
			{
				if (1 == pFeaSelected[j])
				{
					out << j << ":" << ppPProbMatrix[i][j] << "\n";
				}
			}
		}
	}
	else					// output the full information
	{
		// 5.1 the total number of document
		out << iTotalDocNum << endl;

		// 5.2 the prior probability of class 
		out << iClassNum << endl;
		for (int i=0; i<iClassNum; i++)		// classindex:priorprob
		{
			out << i << ":" << pClassPriorProb [i] << "\n";
		}

		// 5.3 the prior probability of feature type: this is NO used in bayes model, record this for more info
		//     and whether this feature is selected or not by any class
		out << iFeaTypeNum << "\n";
		for (int i=0; i<iFeaTypeNum; i++)	// featureId:priorprob:selected or not
		{
			out << i << ":" << pFeaItemPriorProb[i] << ":" << pFeaSelected << "\n";
		}

		// 5.4 the chi-square value for class-feature pair
		for (int i=0; i<iClassNum; i++)
		{
			for (int j=0; j<iFeaTypeNum; j++)
			{
				out << ppChiMatrix[i][j] << "\n";
			}
		}

		// 5.5 the post probability 
		for (int i=0; i<iClassNum; i++)
		{
			for (int j=0; j<iFeaTypeNum; j++)
			{
				out << ppPProbMatrix[i][j] << "\n";
			}
		} 
	}

	// last, release the memory
	delete [] pClassPriorProb;
	delete [] pFeaItemPriorProb;
	for (int i=0; i<iClassNum; i++)
	{
		delete [] ppChiMatrix[i];
	}
	delete [] ppChiMatrix;
	for (int i=0; i<iClassNum; i++)
	{
		delete [] ppPProbMatrix[i];
	}
	delete [] ppPProbMatrix;
	delete [] pFeaSelected;

	return true;
}

bool NaiveBayes::LoadNaiveBayesModel(const char *sFileModel)
{
	ifstream in (sFileModel);
	if (!in)
	{
		cerr << "Can not open the file of " << sFileModel << endl;
		return false;
	}

	// 1. get the class number
	int iClassNum = 0;
	in >> iClassNum;
	if (iClassNum <= 0)
		return false;

	// 2. allocate memory for each class, and get its prior probability
	ClassFeaVec.resize (iClassNum);
	for (int i=0; i<iClassNum; i++)
	{
		ClassFeaVec[i].iClassId = i;
		in >> ClassFeaVec[i].dClassProb;
	}

	// 3. the actual selected feature number
	int iActualSelFeaNum = 0;
	in >> iActualSelFeaNum;
	if (iActualSelFeaNum <= 0)
		return false;
	for (int i=0; i<iClassNum; i++)
	{
		ClassFeaVec[i].FeaVec.resize (iActualSelFeaNum);
	}

	// 4. load the feature and its post probability
	for (int i=0; i<iClassNum; i++)
	{
		for (int j=0; j<iActualSelFeaNum; j++)
		{
			string sTmp;
			in >> sTmp;

			string::size_type iPosition = sTmp.find (":");
			string sFeaId = sTmp.substr (0, iPosition);
			ClassFeaVec[i].FeaVec[j].iFeadId = atoi (sFeaId.c_str());

			iPosition += string (":").length();
			string sProb = sTmp.substr (iPosition);
			ClassFeaVec[i].FeaVec[j].dProb = atof (sProb.c_str());
		}
	}

	return true;
}

bool NaiveBayes::PredictByInputFeas (vector<int> & FeaIdVec, int & iClassId)
{
	if (FeaIdVec.empty())
		return false;

	int iClassNum = ClassFeaVec.size();
	if (iClassNum <= 0)
		return false;

	vector<double> PredProbLogVec (iClassNum, 0.0);	// the predicting probability for each class
	for (int i=0; i<iClassNum; i++)
	{
		PredProbLogVec.at(i) = CalcProbLogByFeaVec (FeaIdVec, ClassFeaVec.at(i).FeaVec);
	}

	// select the class with max probability
	double dMaxProbLog = -DBL_MAX;
	for (int i=0; i<iClassNum; i++)
	{
		if (dMaxProbLog < PredProbLogVec.at(i))
		{
			dMaxProbLog = PredProbLogVec.at(i);
			iClassId = ClassFeaVec.at(i).iClassId;
		}
	}

	return true;
}

bool NaiveBayes::PredictFrmTstCorpus(const char *sFileTestCorpus, string & sSegmenter, const char *sFileOutput)
{
	ifstream in (sFileTestCorpus, ios_base::binary);
	ofstream out (sFileOutput);
	if (!in || !out)
	{
		cerr << "can not open the files" <<endl;
		return false;
	}

	int iSampleNum = 0;
	int iErrNum = 0;
	string sLine;
	while (getline (in, sLine))
	{
		// show something on screen
		if (0 == iSampleNum%1000)
			cout << iSampleNum << "\n";

		// 1 split the sample into class and feature items
		string::size_type iSeg = sLine.find_first_of (sSegmenter);
		string sTmp = sLine.substr (0, iSeg);
		int iClassId = atoi (sTmp.c_str());
		
		// 2 the feature index
		iSeg += sTmp.length();
		sTmp = sLine.substr (iSeg);
		istringstream isLine (sTmp);
		int iFeaId;
		vector<int> FeaIdVec;
		while (isLine >> iFeaId)
			FeaIdVec.push_back (iFeaId);
		sort (FeaIdVec.begin(), FeaIdVec.end());
		FeaIdVec.erase (unique (FeaIdVec.begin(), FeaIdVec.end()), FeaIdVec.end());

		// 3. do prediction
		int iPredClassId = 1;		// the default class index
		PredictByInputFeas (FeaIdVec, iPredClassId);

		// 4. output information to txt
		out << iClassId << ":" << iPredClassId << "\n";

		// 5. update the statistic information
		if (iPredClassId != iClassId)
			iErrNum++;
		iSampleNum++;
	}

	// output the statistic information
	out << "the total number of samples: " << iSampleNum << "\n";
	out << "the error number of prediction: " << iErrNum << "\n";
	out << "the error rate: " << (double)iErrNum / iSampleNum << endl;

	return true;
}

void NaiveBayes::Test (void)
{
	// for debug
	/*Train ( "..\\..\\Data\\ForDataProcessing2\\Train_Tmp.txt", 2, 68407, 
		string(":"), 30000, "..\\..\\Data\\ForDataProcessing2\\BayesModel.txt");
	LoadNaiveBayesModel ("..\\..\\Data\\ForDataProcessing2\\BayesModel.txt");*/

	// train the model
	Train ( "..\\..\\Data\\ForDataProcessing2\\Train_Format.txt", 2, 68407, 
		string(":"), 70000, "..\\..\\Data\\ForDataProcessing2\\BayesModel.txt");
	LoadNaiveBayesModel ("..\\..\\Data\\ForDataProcessing2\\BayesModel.txt");
	PredictFrmTstCorpus ("..\\..\\Data\\ForDataProcessing2\\Test_Rubbishv4_Format.txt", string(":"),
		"..\\..\\Data\\ForDataProcessing2\\Test_Rubbishv4_Log.txt");
	PredictFrmTstCorpus ("..\\..\\Data\\ForDataProcessing2\\Test_weibo_Format.txt", string(":"),
		"..\\..\\Data\\ForDataProcessing2\\Test_weibo_Log.txt");

}
