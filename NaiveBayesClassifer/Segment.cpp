#include "Segment.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iterator>

using namespace std;

RoadNode::RoadNode(void)
{
	iWordIndex = -1;
	iShortestLength = MAX_ROAD_LENGTH;
	iIndex = -1;
	iWordLength = 0;
}

RoadNode::~RoadNode(void)
{
}

void RoadNode::Clear (void)
{
	iWordIndex = -1;
	iShortestLength = MAX_ROAD_LENGTH;
	iIndex = -1;
	iWordLength = 0;
}

CharNode::CharNode(void)
{
	sChar = "" ;
}

CharNode::~CharNode(void)
{
}

void CharNode::Clear( void )
{
	sChar.clear() ;
	RoadVec.clear() ;
}

Segment::Segment(void)
{
	
}

Segment::~Segment(void)
{
}

void Segment::PreFillWordLattice (string &sSentence, vector<CharNode> &WordLattice)
{
	// clear the previous result
	WordLattice.clear ();

	// get each Chinese character in sWord and fill CharVec with them
	for (int i=0; i<(int)sSentence.length(); )
	{
		if (sSentence[i] < 0 && sSentence[i+1] < 0)
		{
			CharNode theNode;
			theNode.sChar = sSentence.substr (i, 2);

			// here makes sure that sChar is in the word list
			if (toWordList.IsWord(theNode.sChar))
				WordLattice.push_back (theNode);
			i += 2;
		}
		else
			// just skip it
			i++;
	}
}

void Segment::ConstructWordLattice(string &sSentence, vector<CharNode> &WordLattice)
{
	PreFillWordLattice (sSentence, WordLattice);

	string sWord ; 
	int iWordIndex ;
	RoadNode tmpNode ;

	vector<CharNode>::iterator p = WordLattice.begin() ;
	while (p != WordLattice.end())
	{
		vector<CharNode>::iterator pBegin = WordLattice.begin() ;
		while (pBegin != p+1)
		{
			vector<CharNode>::iterator pTmp = pBegin;
			int iWordLength = p - pBegin + 1;
			if (iWordLength <= MAX_WORD_LENGTH)
			{
				//get the according 'word' between pBegin and p+1
				while (pTmp != p+1)
				{
					sWord += pTmp->sChar ;
					pTmp++ ;
				}
				if (toWordList.GetWordIdFrmStr (sWord, iWordIndex))
				{
					tmpNode.iWordIndex = iWordIndex;
					tmpNode.iWordLength = iWordLength;
					p->RoadVec.push_back (tmpNode);
				}
				//clear the variables
				sWord.clear();
				tmpNode.Clear();
			}
			pBegin++;
		}
		p++;
	}
}

bool Segment::CaculateRoadNode (vector<CharNode>::iterator pCharNode, 
							    vector<RoadNode>::iterator pRoadNode, vector<CharNode>::iterator pCharNodeBegin)
{
	// vector<CharNode>::difference_type iTotalLength = pCharNode - WordLattice.begin() + 1 ;
	vector<CharNode>::difference_type iTotalLength = pCharNode - pCharNodeBegin + 1;

	if (iTotalLength == pRoadNode->iWordLength)
	{
		//it's the first node in the string of words
		pRoadNode->iShortestLength = 1;
		return true;
	}
	else if (pRoadNode->iWordLength < iTotalLength)
	{
		//it's not the first node on the road
		vector<CharNode>::iterator pBefore = pCharNode - pRoadNode->iWordLength;
		if( pBefore->RoadVec.empty() )
			return false;
		else
		{
			vector<RoadNode>::iterator p = pBefore->RoadVec.begin() ;
			int iIndex = 0 ;
			while (p != pBefore->RoadVec.end())
			{
				if (pRoadNode->iShortestLength > p->iShortestLength + 1)
				{
					//reset the score
					pRoadNode->iShortestLength = p->iShortestLength + 1;
					pRoadNode->iIndex = iIndex ;
				}
				iIndex++ ;
				p++ ;
			}
		}
		return true ;
	}
	else
		//should never reach here
		return false ;
}

bool Segment::TraceBack (vector<int> &WordIdVec, vector<CharNode> &WordLattice)
{
	if (WordLattice.empty())
		return false;

	WordIdVec.clear(); 

	//get the min last element and its position
	vector<CharNode>::iterator p = WordLattice.end() - 1;
	if( p->RoadVec.empty() )
		return false;
	vector<RoadNode>::iterator pRoadNode = 
		min_element (p->RoadVec.begin(), p->RoadVec.end());
	WordIdVec.push_back (pRoadNode->iWordIndex); 

	//trace back
	while (pRoadNode->iIndex != -1)
	{
		//move the pointer
		p -= pRoadNode->iWordLength;
		pRoadNode = p->RoadVec.begin() + pRoadNode->iIndex;
		WordIdVec.push_back (pRoadNode->iWordIndex);
	}

	//reverse the vector
	reverse (WordIdVec.begin(), WordIdVec.end());

	return true ;
}

bool Segment::Init (const char* sFileLex)
{
	return toWordList.LoadWordListFrmTxt (sFileLex);
}

bool Segment::SegmentSentence(string &sSentence, vector<string> &WordVec)
{
	if (sSentence.empty())
		return false;

	// the word lattice
	vector<CharNode> WordLattice;

	//construct the word lattice
	ConstructWordLattice (sSentence, WordLattice);

	//caculate the score of node 
	vector<CharNode>::iterator pCharNode = WordLattice.begin();
	while (pCharNode != WordLattice.end())
	{
		vector<RoadNode>::iterator pRoadNode = pCharNode->RoadVec.begin();
		while( pRoadNode != pCharNode->RoadVec.end())
		{
			CaculateRoadNode (pCharNode, pRoadNode, WordLattice.begin());
			pRoadNode++ ;
		}
		pCharNode++ ;
	}

	//trace back
	vector<int> WordIdVec;
	if (TraceBack (WordIdVec, WordLattice))
	{
		//trun the word index vector into word string vector
		vector<int>::iterator p = WordIdVec.begin();
		while (p != WordIdVec.end())
		{
			string sWord;
			if (toWordList.GetWordStrFrmId (*p, sWord))
				WordVec.push_back (sWord);
			p++;
		}
		return true ;
	}

	return false ;
}

bool Segment::SegmentCorpus(const char *sFileIn, const char *sFileOut)
{
	ifstream in (sFileIn);
	if (!in)
	{
		cerr << "Can not open the file of " << sFileIn << endl;
		return false;
	}

	ofstream out (sFileOut);
	if (!out)
	{
		cerr << "Can not open the file of " << sFileOut << endl;
		return false;
	}

	string sLine;
	int i = 0;
	while (getline (in, sLine))
	{
		if (0 == i % 1000)
			cout << i << "\n";
		i++;

		vector<string> WordVec;
		if (SegmentSentence (sLine, WordVec))
		{
			copy (WordVec.begin(), WordVec.end(), ostream_iterator<string>(out, " "));
			out << "\n";
		}
		else
			out << "\n";
	}

	return true;
}
