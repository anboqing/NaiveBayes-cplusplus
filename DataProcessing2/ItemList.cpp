#include "ItemList.h"

ItemList::ItemList(void)
{
}

ItemList::~ItemList(void)
{
}

bool ItemList::LoadItemListFrmTxt (const char* sFileIn)
{
	ifstream in (sFileIn);
	if (!in)
	{
		cerr << "Can not open the file!" << endl;
		return false;
	}

	ItemVec.clear();

	string sItem;
	while (in >> sItem)
		ItemVec.push_back (sItem);

	sort (ItemVec.begin(), ItemVec.end());

	return true;
}

bool ItemList::IsEmpty (void)
{
	return ItemVec.empty();
}

int ItemList::GetItemNum (void)
{
	return ItemVec.size ();
}

bool ItemList::IsItem (string &sItem)
{
	return binary_search (ItemVec.begin(), ItemVec.end(), sItem);
}

bool ItemList::GetItemIdFrmStr (string &sItem, int &iItemIndex)
{
	vector<string>::iterator p = lower_bound (ItemVec.begin(), ItemVec.end(), sItem);
	if (p != ItemVec.end() && *p == sItem)
	{
		iItemIndex = (int)(p - ItemVec.begin());
		return true;
	}
	else
		return false;
}

void ItemList::GetItemIdVecFrmStrVec (vector<string> &ItemVec, vector<int> &ItemIdVec)
{
	vector<string>::iterator p = ItemVec.begin ();
	while (p != ItemVec.end())
	{
		int iItemIndex = -1;
		if (GetItemIdFrmStr (*p, iItemIndex))
			ItemIdVec.push_back (iItemIndex);
		p++;
	}
}

bool ItemList::GetItemStrFrmId (int iItemIndex, string &sItem)
{
	int iNum = (int)ItemVec.size();
	if (iItemIndex < 0 || iItemIndex >= iNum)
		return false;
	else
	{
		sItem = ItemVec[iItemIndex];
		return true;
	}
}

void ItemList::GetItemStrVecFrmIdVec (vector<int> &ItemIdVec, vector<string> &ItemVec)
{
	vector<int>::iterator p = ItemIdVec.begin ();
	while (p != ItemIdVec.end())
	{
		string sItem;
		if (GetItemStrFrmId (*p, sItem))
			ItemVec.push_back (sItem);
		p++;
	}
}
