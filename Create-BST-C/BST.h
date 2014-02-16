/*******************************************************************************
* Binary Search Tree Header
* Programmer: Nick Shephard
* Date Created: March 1, 20011
* Date Last Modified: March 10, 2011
* Description: 2nd Programming Assignment for CptS 223, Spring 2011.
*******************************************************************************/
#ifndef _BST_H_
#define _BST_H_

//Libraries
#include <string>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

/*******************************************************************************
* Binary Search Tree Class
* Programmer: Nick Shephard
* Date Created: March 1, 20011
* Date Last Modified: March 10, 2011
* Description: Most functions given taken directly out of textbook p.126-134
* Functions for printing(In/Pre/Post-Order), Print traversals, sorting located
* near bottom of header are original
*******************************************************************************/
template <typename Comparable>
class BinarySearchTree
{
private:
	struct BinaryNode
	{
		Comparable element;
		BinaryNode *left;
		BinaryNode *right;

		BinaryNode(const Comparable & theElement, BinaryNode *lt, BinaryNode *rt)
			: element (theElement), left(lt), right(rt){}
	};
	BinaryNode *root;

public:
BinarySearchTree() : root(NULL) {}

bool contains(const Comparable & x) const
{
	return contains(x, root);
}

void insert(const Comparable & x)
{
	insert(x , root);
}

void remove(const Comparable & x)
{
	remove(x, root);
}

bool contains(const Comparable & x, BinaryNode *t) const
{
	if (t == NULL)
	{
		return false;
	}
	else if (x < t->element)
	{
		return contains(x,t->left);
	}
	else if (t->element < x)
	{
		return contains(x, t->right);
	}
	else
	{
		return true;
	}
}

BinaryNode* findMin(BinaryNode *t) const
{
	if (t == NULL)
	{
		return NULL;
	}
	if (t->left == NULL)
	{
		return t;
	}
	return findMin( t->left);
}

BinaryNode * findMax(BinaryNode *t) const
{
	if (t != NULL)
	{
		while (t->right != NULL)
		{
			t = t->right;
		}
	}
	return t;
}

void insert(const Comparable & x, BinaryNode * & t)
{
	if (t == NULL)
	{
		t = new BinaryNode(x, NULL, NULL);
	}
	else if (x < t->element)
	{
		insert(x, t->left);
	}
	else if (t->element < x)
	{
		insert(x, t->right);
	}
	else
	{ ; }
}

void remove(const Comparable & x, BinaryNode * & t)
{

	if (t == NULL)
	{
		return;
	}
	if (x < t->element)
	{
		remove(x, t->left);
	}
	else if (t->element < x)
	{
		remove(x, t->right);
	}
	else if (t->left != NULL && t->right != NULL)
	{
		t->element = findMin(t->right)->element;
		remove(t->element, t->right);
	}
	else
	{
		BinaryNode *oldNode = t;
		t = (t->left != NULL) ? t->left : t->right;
		delete oldNode;
	}
}

~BinarySearchTree()
{
	makeEmpty();
}

void makeEmpty() {
	makeEmpty(root);
}

void makeEmpty(BinaryNode * & t)
{
	if (t != NULL)
	{
		makeEmpty( t->left);
		makeEmpty( t->right);
		delete t;
	}
	t = NULL;
}

const  BinarySearchTree & operator= (const BinarySearchTree & rhs)
{
	if ( this != &rhs)
	{
		makeEmpty();
		root = clone(rhs.root);
	}
	return *this;
}

private:

BinaryNode* clone (BinaryNode *t) const
{
	if (t == NULL)
	{
		return NULL;
	}
	return new BinaryNode(t->element, clone(t->left), clone (t->right));
}

//NEW PRINT FUNCTIONS
/*******************************************************************************
* In / Pre / Post Order Printing Functions
* Programmer: Nick Shephard
* Date Created: March 1, 20011
* Date Last Modified: March 10, 2011
* Description: Parameter:Node(Root) for BinaryTree Struct,
* These functions print the complete elements within a tree in their respective
* order.
*******************************************************************************/
void PrintTreeInOrder(BinaryNode *t) //Left Root Right
{
	if (t != NULL)
	{
		PrintTreeInOrder(t->left);
		cout << " " << t->element;
		PrintTreeInOrder(t->right);
	}
}

void PrintTreePreOrder(BinaryNode *t) //Root Left Right
{
	if (t != NULL)
	{
		cout << " " << t->element;
		PrintTreePreOrder(t->left);
		PrintTreePreOrder(t->right);
	}
}

void PrintTreePostOrder(BinaryNode *t) //Left Right Root
{
	if(t != NULL)
	{
		PrintTreePostOrder(t->left);
		PrintTreePostOrder(t->right);
		cout << " " << t->element;
	}
}

public:
//Print Traversals Function (calls all 3 prints with labels)
void PrintTraversals()
{
	cout << "Pre-Order:";
	PrintTreePreOrder(root);
	cout << endl;
	cout << "In-Order:";
	PrintTreeInOrder(root);
	cout << endl;
	cout << "Post-Order:";
	PrintTreePostOrder(root);
	cout << endl;
}
/*******************************************************************************
* StoreInOrder
* Programmer: Nick Shephard (Help From Teddy Yap)
* Date Created: March 5, 20011
* Date Last Modified: March 10, 2011
* Description: Passes pointer to Binary Search Tree, as well as vector by reference
* for parameters, inserts items from a BST in order into vector v
*******************************************************************************/
void StoreInOrder(BinaryNode *t, vector<Comparable> &v) //Stores elements into vector inOrder (similar to print)
{
	if (t != NULL)
	{
		StoreInOrder(t->left,v);
		v.push_back(t->element);
		StoreInOrder(t->right,v);
	}
}
/*******************************************************************************
* balance
* Programmer: Nick Shephard (Help From Teddy Yap)
* Date Created: March 7, 20011
* Date Last Modified: March 10, 2011
* Description: Passes in a vector(by ref) as parameter in which to insert into a new
* BST in-order. Calls upon StoreInOrder to store current BST, then creates a new
* BST based on the order values were returned from recursiveInsert function. Lastly,
* sets the new sorted BST to the current BST
*******************************************************************************/
void balance(vector <Comparable> &v)
{
	BinaryNode* newT = NULL;	//New BST ptr
	StoreInOrder(root,v);	//Current BST stored into vector v In ascending order
	recursiveInsert(v,0,v.size()-1,newT); //Insert vector values into newT based on given algorithm
	makeEmpty(root);
	root = newT;
}
/*******************************************************************************
* recursiveInsert
* Programmer: Nick Shephard (Help From Teddy Yap)
* Date Created: March 7, 20011
* Date Last Modified: March 10, 2011
* Description: Passes in a vector, a low and high index value, and a BST ref as
* parameters. This function uses the values from the vector to insert into the BST,
* sorting from the lowIndex and HighIndex in relation to the vector input(parameter).
*******************************************************************************/
void recursiveInsert(vector<Comparable> & v,int LowIndex, int HighIndex, BinaryNode *&t) //RecursiveInsert function given in rubric
{
	if (LowIndex <= HighIndex)
	{
		int MidIndex = (LowIndex+HighIndex)/2;
		insert(v.at(MidIndex),t);
		recursiveInsert(v, LowIndex, MidIndex-1,t);
		recursiveInsert(v, MidIndex+1, HighIndex, t);
	}
}
};

#endif
