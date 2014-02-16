/*******************************************************************************
* QUEUE Header and Functions
* Programmer: Nick Shephard
* Date Created: Feb 15, 20011
* Date Last Modified: Apr 29, 2011
* Description: 4th Programming Assignment for CptS 223, Spring 2011.
*******************************************************************************/
#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <string.h>
#include <iostream>

using std::cout;
using std::cin;
using std::endl;
using std::string;

template <class T>
class Queue
{
public:
Queue();
~Queue();
bool enqueue(const T &item);
bool dequeue(T &item);
bool IsEmpty()const;
void print();

private:
	struct node{
		T data;
		node *next;
		/* node(const T & d = T(), node* n = 0) : data(d), node(n) { }; */
	};
	node *Front; //Pointer to Front of Queue
	node *Back; //Pointer to Back of Queue
};

template <class T>
Queue<T>::Queue() //Constructor
{
	Front = NULL;
	Back = NULL;
}

template <class T>
Queue<T>::~Queue() //Destructor
{
	while (!IsEmpty())
	{
		node *temp = Front;
		Front = Front->next;
		delete temp;
	}
}

template <class T>
bool Queue<T>::enqueue(const T &item)
{
	bool status = false;

	node *temp = new node;
	temp->data = item;


	if (Front == NULL) //If new node assign temp to front and back
	{
		Front = temp;
		Back = temp;
	}
	else
	{
		Back->next = temp; //Append temp to rear of list
		Back = temp;
		Back->next = NULL;
	}
	
	status = true;
	return status;
}

template <class T>
bool Queue<T>::dequeue(T &item)
{
	bool status = false;
	node *temp = Front;

	if (temp == NULL)
	{
		return status;
	}
	else
	{
	item = temp->data;
	Front = Front->next;
	delete temp;
	
	status = true;
	}
	Back->next = NULL;
	return status;
}

template <class T>
bool Queue<T>::IsEmpty()const
{
	if (Front == NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

template <class T>
void Queue<T>::print()
{
	node *temp = Front;
	cout << "Contents of Queue: ";
	while (temp != NULL) //iterates through the list until it reaches end
	{
		cout << temp->data << " "; //Prints data in each node followed by a space
		temp = temp->next; //increments 1 node
	}
	cout << "\n";

}
#endif
