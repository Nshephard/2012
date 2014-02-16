/*******************************************************************************
* Project 3: Hash Table Main
* Programmer: Nick Shephard
* Date Created: March 27, 20011
* Date Last Modified: April 3, 2011
* Description: This main performs the operations of creating a Hash Table to store
* each word in the dictionary.txt file and the prints chain length stats of the 
* table. Next the program compares the hash table to words provided from document.txt
* in order to check which word(s) are misspelled.
*******************************************************************************/
#include "HashTable.h"

int main ()
{
	string word;														//Variable to Store words
	int NumberOfWords;													//"" to Store size of dictionary
	bool status;														//"" to error check
		
	ifstream input("dictionary.txt");									//Open Dictionary file into Hash Table
	if (input.good())
	{
		input >> NumberOfWords;
		HashTable<string> A(NumberOfWords);
		for (int i = 0; i < NumberOfWords; i++)
		{
			input >> word;
			status = A.insert(word);									//Insert word into Hash Table /w error checking
			if (status == false)
			{
				cout << "ERROR: Could Not Insert: " << word << endl;
			}
		}
		cout << "Dictionary Hash Table Stats:" << endl;					//Print stats of Hash Table to console
		A.printStats();
		cout << endl;
		cout << "Misspelled Words Found From File:" << endl;			
		ifstream document("document.txt");								//Open file to compare to Hash Table
	
		if (document.good()) 
		{
			while (!document.eof()) 
			{
				document >> word;										
				if (!A.contains(word))									//Check if Hash Table contains word read from file
					cout << word << endl;								//If it DOES NOT contain the word assume its misspelled and print to console
			}
		}
		else 
		{
		}
		input.close();													//Close files
		document.close();
	}
};
