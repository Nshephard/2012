/*******************************************************************************
* Project 3: Hash Table Header File
* Programmer: Nick Shephard
* Date Created: March 27, 20011
* Date Last Modified: April 3, 2011
* Description: Header contains given templated Hash Table Class from text pg. 188-191.
* Also includes a PrintStats functions (that prints min/mean/max chain length to console) 
* and a Hash function for strings provided from text pg. 187.
*******************************************************************************/
#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include <list>								//Libraries
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

using namespace std;

template <typename HashedObj>
class HashTable
{
public:
	explicit HashTable (int size=101)			//Constructor
	{
		theLists.resize(size);
	}

	bool contains (const HashedObj & x) const	 //Contains
	{
		const list<HashedObj> & whichList = theLists[myhash(x)];
		return find( whichList.begin(), whichList.end(), x) != whichList.end();
	}

	bool insert (const HashedObj & x)			//Insert
	{
		list<HashedObj> & whichList = theLists[myhash(x)];
		if (find(whichList.begin(), whichList.end(), x) != whichList.end())
		{
			return false;
		}
		whichList.push_back(x);
		return true;
	}

	void printStats ()
	{
		unsigned int min = 1000;							//Assuming at least 1 chain will be less than 1000 :)
		unsigned int max = 0;
		unsigned int sum = 0;
		unsigned int mean = 0;
		for (unsigned int i = 0; i < theLists.size(); i++)	//Scan through entire table of lists
		{
			if (theLists[i].size() < min)
			{
				min = theLists[i].size();					//Min
			}
			if (theLists[i].size() > max)					//Max
			{
				max = theLists[i].size();
			}
			sum += theLists[i].size();
		}
		mean = theLists.size() / sum;						//Mean	
		cout << "Minimum Chain Length: " << min << endl;	//Print
		cout << "Maximum Chain Length: " << max << endl;
		cout << "Mean Chain Length: " << mean << endl;
	}
private:
	vector<list<HashedObj> > theLists;

	int myhash( const HashedObj & x) const 
	{
		int hashVal = hash1(x);

		hashVal %= theLists.size();
		if(hashVal < 0)
		{
			hashVal += theLists.size();
		}
		return hashVal;
	}
};

int hash1(const string & key) //Given Hash Function
{
	int hashVal = 0;

	for (unsigned int i = 0; i < key.length(); i++)
	{
		hashVal = 37 * hashVal + key[i];
	}
	return hashVal;
}

#endif
