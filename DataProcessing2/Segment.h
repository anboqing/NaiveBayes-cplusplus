#pragma once

#include "ItemList.h"

using namespace std;

// represent a node on one of roads in word lattice
class RoadNode
{
public:
	// the max road length while segmenting
	const static int MAX_ROAD_LENGTH = INT_MAX;
	//the word index
	int iWordIndex;
	//the word length(not the bit number, a chinese character's length is 1)
	int iWordLength;
	//the chosen road which before this node
	int iIndex;
	//the shortest length of road until now
	int iShortestLength;

public:
	bool operator < (const RoadNode &theNode) const
	{
		if (iShortestLength < theNode.iShortestLength)
			return true;
		else
			return false;
	}
	bool operator == (const RoadNode &theNode) const
	{
		if (iShortestLength == theNode.iShortestLength)
			return true;
		else
			return false;
	}

	void Clear (void);

public:
	RoadNode(void);
	~RoadNode(void);
};


// represent a charecter node from which the word lattice is built
class CharNode
{
public:
	//the single chinese character
	string sChar ;
	//the according road node
	vector<RoadNode> RoadVec ;

public:
	void Clear( void ) ;

public:
	CharNode(void);
	~CharNode(void);
};


/*****************************************************************************/
// this is to segment the corpus by the 'minimal number of element' algorithm.
/*****************************************************************************/
class Segment
{
private:
	// the max word length to segment
	const static int MAX_WORD_LENGTH = 15;
	// the item list object containing the lexicon for segmentation
	ItemList * pWordList;

private:
	// decompose the sentence into seperate Chinese character, 
	// and pre-fill WordLattice
	void PreFillWordLattice (string &sSentence, vector<CharNode> &WordLattice);
	// construct the word lattice
	void ConstructWordLattice (string &sSentence, vector<CharNode> &WordLattice);
	// caculate the score of each node in the word lattice, 
	// return false if any error occurs
	bool CaculateRoadNode (vector<CharNode>::iterator pCharNode, 
		vector<RoadNode>::iterator pRoadNode, vector<CharNode>::iterator pCharNodeBegin);
	// trace back the road and get the word series, return false if no road(imposible)
	bool TraceBack (vector<int> &WordIdVec, vector<CharNode> &WordLattice);

public:
	// initialize 
	bool Init (ItemList * pTheItemList);
	// input a sentence of characters, segment them into word string vector
	bool SegmentSentence (string &sSentence, vector<string> &WordVec);
	// input a corpus, segment each sentence in it
	bool SegmentCorpus (const char* sFileIn, const char* sFileOut);

public:
	Segment(void);
	~Segment(void);
};
