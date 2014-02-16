/*******************************************************************************
* Binary Search Tree Main
* Programmer: Nick Shephard
* Date Created: March 3, 20011
* Date Last Modified: March 10, 2011
* Description: This main performs the operations of reading input from a file and
* storing them to BSTs(unsorted/sorted), calling functions declared in BST.h
* to perform these operations and print to console.
*******************************************************************************/
//Libraries
#include "BST.h"
#include <string>
#include <fstream>

using namespace std;

int main()
{
	int NumberOfTrees;									//variable for storing 1st input from file (the total number of BSTs)
	int TreeIterator = 1;								//Iterator for printing correct tree order(entire trees)
	vector<int> vect;									//vector for calling s

	ifstream input("input.txt");						//File to read from ** CHANGE HERE FOR DIFFERENT .txt FILES TO READ FROM **

	if (input.good()) {									//if a value is read
		input >> NumberOfTrees;							//1st value (number of total trees)
		for (int t = 1; t <= NumberOfTrees; t++) { 
			BinarySearchTree<int> B;					//Binary Search tree in which values are stored/altered
			int numValues;								//1st value from each line read (number of elements in tree on the line)
			input >> numValues;
			for (int i = 1; i <= numValues; i++) {		//Insert each value read into tree B
				int value;								//Values of numbers(elements) read to put into tree
				input >> value;
				B.insert(value); 
			}
			cout << "Tree " << TreeIterator << ":"<<endl; //Print traverals before sort of tree B
			B.PrintTraversals();
			cout << endl;
			B.balance(vect);							 //Balance tree B using balance function
			cout << "Balanced Tree " << TreeIterator << ":" << endl;
			B.PrintTraversals();						 //Print NOW BALANCED tree
			B.makeEmpty();
			cout << endl;
			TreeIterator++;								//Increase tree itr for printing purposes
		}
	}
	else
		cout << "Error reading file." << endl;			//Error in opening file

	string temp;										//requested input(by Teddy) to end program
	getline(cin, temp);
}
